/*
 * Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 * Distributed under the terms of the GPL v3 license, available in the file
 * LICENSE.
 */

#include "fuel_tank.hpp"

#include <math.h>

#include "main.h"

VtolFuelTank::VtolFuelTank() {}

int8_t VtolFuelTank::init(uint8_t tank_id, uint32_t min_angle, uint32_t max_angle, uint8_t volume_cm3, bool is_reserved) {
  char buffer[90];
  _logger.init("FuelTank");

  int hal_status = 0;
  volume = volume_cm3;
  
  if (hal_status != 0) {
    sprintf(buffer, "I2C %d", hal_status);
    _logger.log_error(buffer);
    return hal_status;
  }

  hal_status = isDeviceReady(I2C_AS5600, 100);
  if (hal_status != 0) {
    sprintf(buffer, "I2C_isDevReady %d", hal_status);
    _logger.log_error(buffer);
  }

  uint8_t transfer_id = 0;
  _tank_info.available_fuel_volume_percent = 0;
  _tank_info.available_fuel_volume_cm3 = 0.0;
  _tank_info.fuel_consumption_rate_cm3pm = .0;
  _tank_info.fuel_temperature = NAN;
  _tank_info.fuel_tank_id = tank_id;
  _tank_info.reserved = is_reserved;

  min_value = min_angle;
  max_value = max_angle;
  as5600_error_t as5600_status = this->_as5600.init(min_value, max_value);

  if (as5600_status != 0) {
    sprintf(buffer, "AS5600_INIT: %d", as5600_status);
    _logger.log_error(buffer);
    return 1;
  } else {
    sprintf(buffer, "AS5600_INIT: %d", this->_as5600.data.mag_status);
    _logger.log_debug(buffer);
  }
  _logger.log_info("INIT COMP");
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
  uint8_t status = _as5600.get_angle_data(ZPOS, &zero_val);
  if (status != 0) {
    sprintf(buffer, "GET_ZERO: %d", status);
    _logger.log_error(buffer);
    return status;
  }

  status = update_data();

  if (status != 0) {
    sprintf(buffer, "UPDATE: %d", status);
    _logger.log_error(buffer);
    return status;
  }

  status = dronecan_equipment_ice_fuel_tank_status_publish(&_tank_info,
                                                           &_transfer_id);
  _transfer_id++;

  if (status != 0) {
    sprintf(buffer, "CAN: %d", status);
    _logger.log_error(buffer);
  }
  return status;
}

int8_t VtolFuelTank::update_data() {
  uint32_t crnt_time_ms = HAL_GetTick();

  // TODO: change to 0.2 Hz
  if (crnt_time_ms < _last_update_time_ms + 200) {
    return 0;
  }
  _last_update_time_ms = HAL_GetTick();


  as5600_error_t as5600_stat = _as5600.get_angle_data(RAW_ANGLE, &_as5600.data.raw_angle);
  uint16_t angle = AS5600_12_BIT_MASK & _as5600.data.raw_angle;
  _as5600.data.angle = uint16_t(((float)angle) / 4095.0f * 360.0f);

  if (as5600_stat != AS5600_SUCCESS) {
    return as5600_stat;
  }

  _tank_info.available_fuel_volume_percent =
      (_as5600.data.angle - min_value)*100 / max_value;

  _tank_info.available_fuel_volume_cm3 =
      (_tank_info.available_fuel_volume_percent * volume)/100.0f;

  _tank_info.fuel_consumption_rate_cm3pm += _as5600.data.angle - min_value;

  return 0;
}

int8_t VtolFuelTank::set_zero(uint16_t val) {
  uint32_t crnt_time_ms = HAL_GetTick();

  // TODO: change to 1 Hz
  if (crnt_time_ms < _last_set_time_ms + 200) {
    return 0;
  }
  _last_set_time_ms = HAL_GetTick();
  return _as5600.set_zero_position(val);
}