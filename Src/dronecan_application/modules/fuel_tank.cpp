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


VtolFuelTank::VtolFuelTank() {
}


int8_t VtolFuelTank::init(uint8_t tank_id, uint16_t is_reserved) {
    _tank_info.available_fuel_volume_cm3=0.0;
    _tank_info.available_fuel_volume_percent=0.0;
    _tank_info.fuel_consumption_rate_cm3pm = 1.0;
    _tank_info.fuel_tank_id=0;
    _tank_info.fuel_temperature=200;
    _tank_info.reserved = is_reserved;
    
    uint8_t transfer_id = tank_id;

    this->as5600.init();

    return 0;
}

void VtolFuelTank::process() {
    uint32_t crnt_time_ms = HAL_GetTick();

    // TODO: change to 1 Hz
    if (crnt_time_ms < _last_publish_time_ms + 200) {
        return;
    }

    update_data();
    const int8_t can_publish_status = dronecan_equipment_ice_fuel_tank_status_publish(&_tank_info, &_transfer_id);

    if (can_publish_status != 0) {
        // TODO: log error here
    }

    _transfer_id++;
    _last_publish_time_ms = HAL_GetTick();
}

void VtolFuelTank::update_data(){
    uint32_t crnt_time_ms = HAL_GetTick();

    // TODO: change to 0.2 Hz
    if (crnt_time_ms < _last_update_time_ms + 200) {
        return;
    }
    
    const int8_t as5600_status = this->as5600.get_data(RAW_ANGLE,this->as5600.data.raw_angle, 2);
    if (as5600_status != 0) {
        // TODO: log error here
    }
    this->_tank_info.available_fuel_volume_percent = as5600.data.raw_angle;
    if (_tank_info.fuel_consumption_rate_cm3pm == 0){
        _tank_info.fuel_consumption_rate_cm3pm = 1;
    }
    else{
        _tank_info.fuel_consumption_rate_cm3pm = 0;

    }

    _last_update_time_ms = HAL_GetTick();
}