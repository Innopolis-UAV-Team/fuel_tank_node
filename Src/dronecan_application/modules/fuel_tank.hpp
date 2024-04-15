/*
* Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/

#ifndef SRC_MODULES_FUEL_TANK_HPP_
#define SRC_MODULES_FUEL_TANK_HPP_
#include "params.hpp"
#include <stdint.h>
#include "uavcan/equipment/ice/FuelTankStatus.h"
#include "periphery/as5600/as5600.hpp"
#include "uavcan/protocol/debug/LogMessage.h"
#include "uavcan/equipment/esc/RawCommand.h"
#include "logger.hpp"


class VtolFuelTank {
public:
    VtolFuelTank();
    int8_t init(uint8_t tank_id, uint32_t angle_full, uint32_t angle_empty, uint32_t volume_cm3);
    int8_t process();
    // used for i2c ERROR
    uint8_t  n_sec_waiting = 0;

    // angle value bounds in deg
    uint16_t min_value = 0;
    uint16_t max_value = 0;

    // fuel tank volume in cm3
    uint32_t volume = 1;

private:
    uint8_t update_params();
    uint8_t update_data();
    int8_t calibrate();
    static void raw_command_callback(CanardRxTransfer* transfer);

    float angles_buffer[15] = {};
    float filtered_angle;
    uint8_t buffer_ctr;
    uint8_t window_size;
    static bool is_vehicle_armed;
    static uint32_t ttl_cmd;
    static uint32_t cmd_end_time_ms;    
    uint32_t _last_publish_time_ms{0};
    uint32_t _last_update_time_ms{0};
    uint32_t _last_set_time_ms{0};
    uint8_t _transfer_id = 0;
    uint8_t _log_transfer_id = 0;

    Logger _logger{};
    As5600Periphery _as5600{};
    FuelTankStatus_t _tank_info{};

    void _apply_angle_boundaries();
};


#endif  // SRC_MODULES_FUEL_TANK_HPP_