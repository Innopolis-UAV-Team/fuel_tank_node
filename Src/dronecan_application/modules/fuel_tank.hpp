/*
* Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/

#ifndef SRC_MODULES_FUEL_TANK_HPP_
#define SRC_MODULES_FUEL_TANK_HPP_

#include <stdint.h>
#include "uavcan/equipment/ice/FuelTankStatus.h"
#include "periphery/as5600/as5600.hpp"
#include "uavcan/protocol/debug/LogMessage.h"
#include "logger.hpp"
#include "logging.h"


class VtolFuelTank {
public:
    VtolFuelTank();
    int8_t init(uint8_t tank_id, uint32_t angle_full, uint32_t angle_empty, uint8_t volume_cm3, bool is_reserved=0);
    int8_t process();
    int8_t update_data();
    int8_t calibrate();
    int8_t set_zero(uint16_t val);

    // uint16_t full_tank_angle;
    // uint16_t empt_tank_angle;
    uint16_t min_value = 0;
    uint16_t max_value = 0;
    uint16_t volume = 1;
private:
    uint32_t _last_publish_time_ms{0};
    uint32_t _last_set_time_ms{0};
    uint8_t _transfer_id = 0;
    uint8_t _log_transfer_id = 0;

    Logger _logger{};
    As5600Periphery _as5600{};
    FuelTankStatus_t _tank_info{};
};


#endif  // SRC_MODULES_FUEL_TANK_HPP_