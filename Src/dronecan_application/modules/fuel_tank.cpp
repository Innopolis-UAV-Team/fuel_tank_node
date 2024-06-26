/*
 * Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 * Distributed under the terms of the GPL v3 license, available in the file
 * LICENSE.
 */

#include "fuel_tank.hpp"

#include <math.h>

#include "main.h"

uint32_t VtolFuelTank::cmd_end_time_ms = 0;
uint32_t VtolFuelTank::ttl_cmd = 1000;
bool VtolFuelTank::is_vehicle_armed = false;

void VtolFuelTank::_apply_angle_boundaries() {
    if (filtered_angle < min_value) {
        filtered_angle = min_value;
    } else if (filtered_angle > max_value) {
        filtered_angle = max_value;
    }
}

VtolFuelTank::VtolFuelTank() {}

int8_t VtolFuelTank::init(uint8_t tank_id, uint32_t angle_full,
                          uint32_t angle_empty, uint32_t volume_cm3) {
    
    
    char buffer[90];
    module_status = ModuleStatus::MODULE_OK;

    _logger.init("FuelTank");
    int hal_status = 0;
    volume = volume_cm3;
    hal_status = isDeviceReady(I2C_AS5600, 100);
    if (hal_status != 0) {
        sprintf(buffer, "I2C_isDevReady %d", hal_status);
        _logger.log_debug(buffer);
        module_status = ModuleStatus::MODULE_ERROR;
    }

    _tank_info.available_fuel_volume_percent = 0;
    _tank_info.available_fuel_volume_cm3 = 0.0;
    _tank_info.fuel_consumption_rate_cm3pm = .0;
    _tank_info.fuel_temperature = NAN;
    _tank_info.fuel_tank_id = tank_id;

    min_value = 0;
    max_value = 360;

    as5600_error_t as5600_status = _as5600.init(min_value, max_value);

    if (as5600_status != 0) {
        sprintf(buffer, "AS5600_INIT: %d", as5600_status);
        _logger.log_debug(buffer);
        module_status = ModuleStatus::MODULE_ERROR;
        return -1;
    } else {
        sprintf(buffer, "AS5600_INIT: %d", _as5600.data.mag_status);
        _logger.log_debug(buffer);
    }

    auto sub_id =
        uavcanSubscribe(UAVCAN_EQUIPMENT_ESC_RAWCOMMAND, raw_command_callback);
    if (sub_id < 0) _logger.log_debug("sub_id < 0");

    _logger.log_debug("INIT COMP");
    return 0; 
}

int8_t VtolFuelTank::update_params() {
    module_status = ModuleStatus::MODULE_OK;

    are_params_correct = true;
    uint16_t angle_full = paramsGetIntegerValue(PARAM_FUEL_TANK_FULL);
    uint16_t angle_empty = paramsGetIntegerValue(PARAM_FUEL_TANK_EMPTY);
    volume = paramsGetIntegerValue(PARAM_FUEL_TANK_VOLUME);

    if (angle_full == angle_empty) {
        are_params_correct = false;
        _logger.log_debug("empty_tank_enc_deg == full_tank_enc_deg");
        module_status = ModuleStatus::MODULE_WARN;
        return -1;
    }

    uint16_t new_min_value;
    uint16_t new_max_value;

    if (angle_full < angle_empty) {
        _as5600.data.dir = 1;  //  counterclockwise
        new_min_value = angle_full;
        new_max_value = angle_empty;
    } else {
        _as5600.data.dir = 0;  // clockwise
        new_min_value = angle_empty;
        new_max_value = angle_full;
    }

    min_value = new_min_value;
    max_value = new_max_value;

    return 0;
}

void VtolFuelTank::process() {
    uint32_t crnt_time_ms = HAL_GetTick();

    if (crnt_time_ms < _last_update_time_ms + 100) return;

    int8_t status = update_data();
    _last_update_time_ms = HAL_GetTick();

    static uint32_t next_error_publish = 0;

    if (status != 0 && next_error_publish < HAL_GetTick()) {
        _logger.log_debug("NO_DATA");
        next_error_publish = crnt_time_ms + 10000;
    }

    if (module_status != ModuleStatus::MODULE_OK) {
        return;
    }

    if (crnt_time_ms < _last_publish_time_ms + 1000) return;

    status = dronecan_equipment_ice_fuel_tank_status_publish(&_tank_info,
                                                            &_transfer_id);
    _transfer_id++;
    
    _last_publish_time_ms = HAL_GetTick();

    if (cmd_end_time_ms < HAL_GetTick()) is_vehicle_armed = false;
    if (is_vehicle_armed) _logger.log_debug("armed");
}

int8_t VtolFuelTank::update_data() {
    static uint32_t next_update_time = 0;
    as5600_error_t as5600_stat = AS5600_BAD_PARAMETER;

    if (HAL_GetTick() > next_update_time) {
        if (update_params() == 0) {
            as5600_stat = _as5600.init(min_value, max_value);
            if (as5600_stat != AS5600_SUCCESS) {
                module_status = ModuleStatus::MODULE_ERROR;
                return as5600_stat;
            }
        }
        next_update_time = HAL_GetTick() + 1000;
    }

    as5600_stat =
        _as5600.get_angle_data(RAW_ANGLE, &_as5600.data.raw_angle);

    if (as5600_stat != AS5600_SUCCESS) {
        module_status = ModuleStatus::MODULE_ERROR;
        return as5600_stat;
    }

    uint16_t angle = AS5600_12_BIT_MASK & _as5600.data.raw_angle;

    _as5600.data.angle = ((float)angle) / 4095.0f * 360.0f;

    // Moving average on values
    if (is_vehicle_armed) window_size = 15;
    else window_size = 5;
    movingAverage(&filtered_angle, _as5600.data.angle, window_size);

    uavcanSetVendorSpecificStatusCode((uint16_t)(filtered_angle));

    _apply_angle_boundaries();

    _tank_info.available_fuel_volume_percent =
          (filtered_angle - min_value) * 100 / (max_value - min_value);

    if (_as5600.data.dir == 1) {
        _tank_info.available_fuel_volume_percent =
            100 - _tank_info.available_fuel_volume_percent;
    }

    _tank_info.available_fuel_volume_cm3 =
        (_tank_info.available_fuel_volume_percent * volume) / 100.0f;

    return 0;
}

void VtolFuelTank::raw_command_callback(CanardRxTransfer* transfer) {
    RawCommand_t command;
    cmd_end_time_ms = HAL_GetTick() + ttl_cmd;

    int8_t recieved_n_ch =
        dronecan_equipment_esc_raw_command_deserialize(transfer, &command);
    if (recieved_n_ch != 0) {
        is_vehicle_armed = false;
        for (uint8_t i = 0; i < recieved_n_ch; i++) {
            if (command.raw_cmd[i] >= 0) {
                is_vehicle_armed = true;
            }
        } 
    }
}
