// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#include "fuel_tank.hpp"
#include <math.h>
#include "uavcan/protocol/debug/LogMessage.h"
#include "logging.h"
#include "main.h"

VtolFuelTank::VtolFuelTank() {
}

int8_t VtolFuelTank::init(uint8_t tank_id, uint16_t is_reserved) {
    
    set_source(this->init_mes, "fuel_init");
    set_source(this->proc_mes, "fuel_proc");
    

    int hal_status = 0;
    // HAL_StatusTypeDef hal_status = HAL_I2C_Init(&hi2c1);

    if (hal_status != 0){
        char buffer[90];
        sprintf(buffer, "I2C_init_status %d", hal_status);
        set_text(this->init_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
        _log_transfer_id++;
        return hal_status;
    }

    hal_status = isDeviceReady(I2C_AS5600, 100);
    if (hal_status != 0){
        char buffer[90];
        sprintf(buffer, "I2C_isDeviceReady_status %d", hal_status);
        set_text(this->init_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
        _log_transfer_id++;
    }

    uint8_t transfer_id = 0;
    _tank_info.available_fuel_volume_percent = 0;
    _tank_info.available_fuel_volume_cm3 = 0.0;
    _tank_info.fuel_consumption_rate_cm3pm = 1.0;
    _tank_info.fuel_temperature = NAN;
    _tank_info.fuel_tank_id = tank_id;
    _tank_info.reserved = is_reserved;
    
    hal_status = dronecan_equipment_ice_fuel_tank_status_publish(&_tank_info, &transfer_id);

    if (hal_status != 0){
        set_text(this->init_mes, "PUB_ERROR");
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
        _log_transfer_id++;
        return hal_status;
     }  

    as5600_error_t as5600_status = this->as5600.init();

    if (as5600_status != 0){
        char buffer[90];
        sprintf(buffer, "AS5600_INIT_ERROR: %d", as5600_status);
        set_text(this->init_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
        _log_transfer_id++;
        return 1;
    } else {
        char buffer[90];
        sprintf(buffer, "AS5600_INIT: %d", this->as5600.data.mag_status);
        set_text(this->init_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
    }
    _transfer_id++;
    return 0;
}

int8_t VtolFuelTank::process() {
    uint32_t crnt_time_ms = HAL_GetTick();

    // TODO: change to 1 Hz
    if (crnt_time_ms < _last_publish_time_ms + 200) {
        return 0;
    }

    _last_publish_time_ms = HAL_GetTick();

    uint8_t hal_status = 0;
    
    // TODO: fix the read_16_reg()
    hal_status = update_data();
    
    if (hal_status != 0){
        char buffer[90];
        sprintf(buffer, "UPDATE hal status: %d", hal_status);
        set_text(this->proc_mes, buffer);

        dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
        _log_transfer_id++;
        return hal_status;
    }

    hal_status = dronecan_equipment_ice_fuel_tank_status_publish(&_tank_info, &_transfer_id);
    _transfer_id++;

    if (hal_status != 0){
        char buffer[90];
        sprintf(buffer, "CAN publish error: %d", hal_status);
        set_text(this->proc_mes, buffer);

        dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
        _log_transfer_id++;
    } 
    return hal_status;
}

int8_t VtolFuelTank::update_data(){
    uint32_t crnt_time_ms = HAL_GetTick();

    // TODO: change to 0.2 Hz
    if (crnt_time_ms < _last_update_time_ms + 200) {
        return 0;
    }
    _last_update_time_ms = HAL_GetTick();

    as5600_error_t as5600_stat = as5600.get_angle_data(RAW_ANGLE, &as5600.data.raw_angle);
    // as5600_error_t as5600_stat = as5600.get_angle_data(ZPOS, &as5600.data.start_angle);

    if (as5600_stat!=AS5600_SUCCESS){
        return as5600_stat;
    }

    this->_tank_info.available_fuel_volume_cm3 = as5600.data.raw_angle;
    this->_tank_info.available_fuel_volume_percent = as5600.data.start_angle;
    this->_tank_info.fuel_consumption_rate_cm3pm += 0.1;
    return 0;
}