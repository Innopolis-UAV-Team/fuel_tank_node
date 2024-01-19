/*
* Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/ 


#include "fuel_tank.hpp"
#include <math.h>
#include "main.h"

VtolFuelTank::VtolFuelTank() {
}

int8_t VtolFuelTank::init(uint8_t tank_id, uint16_t is_reserved) {
    
    char buffer[90];
    logger.init("FuelTank");

    int hal_status = 0;

    if (hal_status != 0){
        sprintf(buffer, "I2C %d", hal_status);
        logger.log_error(buffer);
        return hal_status;
    }

    hal_status = isDeviceReady(I2C_AS5600, 100);
    if (hal_status != 0){
        sprintf(buffer, "I2C_isDevReady %d", hal_status);
        logger.log_error(buffer);
    }

    uint8_t transfer_id = 0;
    _tank_info.available_fuel_volume_percent = 0;
    _tank_info.available_fuel_volume_cm3 = 0.0;
    _tank_info.fuel_consumption_rate_cm3pm = 1.0;
    _tank_info.fuel_temperature = NAN;
    _tank_info.fuel_tank_id = tank_id;
    _tank_info.reserved = is_reserved;
    
    // hal_status = dronecan_equipment_ice_fuel_tank_status_publish(&_tank_info, &transfer_id);
    // _transfer_id++;

    // if (hal_status != 0){
    //     logger.log_error("PUB");
    //     return hal_status;
    //  }  

    as5600_error_t as5600_status = this->as5600.init();

    if (as5600_status != 0){
        sprintf(buffer, "AS5600_INIT: %d", as5600_status);
        logger.log_error(buffer);
        return 1;
    } else {
        sprintf(buffer, "AS5600_INIT: %d", this->as5600.data.mag_status);
        logger.log_debug(buffer);
    }
    logger.log_info("INIT COMP");
    return 0;
}

int8_t VtolFuelTank::process() {
    uint32_t crnt_time_ms = HAL_GetTick();

    // TODO: change to 1 Hz
    if (crnt_time_ms < _last_publish_time_ms + 1000) {
        return 0;
    }

    char buffer[90];
    _last_publish_time_ms = HAL_GetTick();

    uint16_t zero_val = 0;
    uint8_t status = as5600.get_angle_data(ZPOS, &zero_val);
    if (status != 0){
        sprintf(buffer, "GET_ZERO: %d", status);
        logger.log_error(buffer);
        return status;
    } 

    status = update_data();
    
    if (status != 0){
        sprintf(buffer, "UPDATE: %d", status);
        logger.log_error(buffer);
        return status;
    }

    status = dronecan_equipment_ice_fuel_tank_status_publish(&_tank_info, &_transfer_id);
    _transfer_id++;

    if (status != 0){
        sprintf(buffer, "CAN: %d", status);
        logger.log_error(buffer);
    } 
    return status;
}

int8_t VtolFuelTank::update_data(){
    uint32_t crnt_time_ms = HAL_GetTick();

    // TODO: change to 0.2 Hz
    if (crnt_time_ms < _last_update_time_ms + 200) {
        return 0;
    }
    _last_update_time_ms = HAL_GetTick();

    as5600_error_t as5600_stat = as5600.get_angle_data(RAW_ANGLE, &as5600.data.raw_angle);
    uint16_t angle = AS5600_12_BIT_MASK & as5600.data.raw_angle;
    as5600.data.angle = ((float)angle)/4095.0f*360.0f;

    if (as5600_stat!=AS5600_SUCCESS){
        return as5600_stat;
    }

    this->_tank_info.available_fuel_volume_percent = as5600.data.raw_angle;
    this->_tank_info.available_fuel_volume_cm3 = as5600.data.angle;
    this->_tank_info.fuel_consumption_rate_cm3pm += 0.1;
    return 0;
}

int8_t VtolFuelTank::set_zero(uint16_t val){
    uint32_t crnt_time_ms = HAL_GetTick();

    // TODO: change to 1 Hz
    if (crnt_time_ms < _last_set_time_ms + 200) {
        return 0;
    }
    _last_set_time_ms = HAL_GetTick();
    return as5600.set_zero_position(val);
}