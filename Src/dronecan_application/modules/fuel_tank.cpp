// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#include "fuel_tank.hpp"
#include <math.h>
#include "dronecan.h"
#include "main.h"
#include "params.hpp"
#include "string_params.hpp"
#include "string_params.hpp"
#include "storage.h"
#include "uavcan/protocol/debug/LogMessage.h"
#include "logging.h"

VtolFuelTank::VtolFuelTank() {
}

// ES - status publish error 
// EP - log periphery status publish error
// EP - update status publish error
int8_t VtolFuelTank::init(uint8_t tank_id, uint16_t is_reserved) {
    set_source(this->init_mes, "fuel_init");
    set_source(this->proc_mes, "fuel_proc");
    
    uint8_t transfer_id = 0;
    _tank_info.available_fuel_volume_percent = 0;
    _tank_info.available_fuel_volume_cm3 = 0.0;
    _tank_info.fuel_consumption_rate_cm3pm = 1.0;
    _tank_info.fuel_temperature = NAN;
    _tank_info.fuel_tank_id = tank_id;
    _tank_info.reserved = is_reserved;
    
    const int8_t can_publish_status = dronecan_equipment_ice_fuel_tank_status_publish(&_tank_info, &transfer_id);

    if (can_publish_status != 0){
        set_text(this->init_mes, "PUB_ERROR");
        
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
        _log_transfer_id++;
     }  
    //  else {
    //     init_mes.text[0] = 'O';
    //     init_mes.text[1] = 'K';
    //     init_mes.text[2] = 'S';
    //     init_mes.text[3] = as5600.data.mag_status;
    //     init_mes.text_size = 4;
    //     dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
    // }

    int8_t hal_status = this->as5600.init();

    if (hal_status != 0){
        set_text(this->init_mes, "AS_INIT_ERROR");
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
        _log_transfer_id++;
    }
    else {
        char buffer[90];
        sprintf(buffer, "OK_AS5600_INIT: %d", this->as5600.data.mag_status);
        set_text(this->init_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
}
    _transfer_id++;
    return 0;
}

void VtolFuelTank::process() {
    uint32_t crnt_time_ms = HAL_GetTick();

    // TODO: change to 1 Hz
    if (crnt_time_ms < _last_publish_time_ms + 200) {
        return;
    }

    uint8_t hal_status = update_data();

    if (hal_status != 0){
        char buffer[90];
        sprintf(buffer, "Update hal status: %d", hal_status);
        set_text(this->proc_mes, buffer);

        dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
        _log_transfer_id++;
    }//  else {
    //     proc_mes.text[0] = 'O';
    //     proc_mes.text[1] = 'K';
    //     proc_mes.text[2] = 'U';
    //     proc_mes.text_size = 4;
    //     dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
    // }

    const int8_t can_publish_status = dronecan_equipment_ice_fuel_tank_status_publish(&_tank_info, &_transfer_id);

    if (can_publish_status != 0){
        char buffer[90];
        sprintf(buffer, "CAN publish error: %d", can_publish_status);
        set_text(this->proc_mes, buffer);

        dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
        _log_transfer_id++;
    } //else {
    //     proc_mes.text[0] = 'O';
    //     proc_mes.text[1] = 'K';
    //     proc_mes.text[2] = as5600.data.raw_angle;
    //     proc_mes.text[3] = as5600.data.start_angle;
    //     proc_mes.text[4] = 'F';
    //     proc_mes.text_size = 5;
    //     dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
    // }

    _transfer_id++;
    _last_publish_time_ms = HAL_GetTick();
}

int8_t VtolFuelTank::update_data(){
    uint32_t crnt_time_ms = HAL_GetTick();

    // TODO: change to 0.2 Hz
    if (crnt_time_ms < _last_update_time_ms + 200) {
        return 0;
    }

    as5600.get_angle_data(RAW_ANGLE, &as5600.data.raw_angle);
    as5600.get_angle_data(ZPOS, &as5600.data.start_angle);
    this->_tank_info.available_fuel_volume_cm3 += as5600.data.max_value;
    this->_tank_info.available_fuel_volume_percent = as5600.data.start_angle;
    this->_tank_info.fuel_consumption_rate_cm3pm += 0.1;
    _last_update_time_ms = HAL_GetTick();
    return 0;
}