// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_MODULES_FUEL_TANK_HPP_
#define SRC_MODULES_FUEL_TANK_HPP_

#include <stdint.h>
#include "uavcan/equipment/ice/FuelTankStatus.h"
#include "periphery/as5600/as5600.hpp"
#include "uavcan/protocol/debug/LogMessage.h"

// #ifdef __cplusplus
// extern "C" {
// #endif


class VtolFuelTank {
public:
    VtolFuelTank();
    int8_t init(uint8_t tank_id, uint16_t is_reserved);
    void process();
    int8_t update_data();

private:
    As5600Periphery as5600{};
    uint32_t _last_publish_time_ms{0};
    uint32_t _last_update_time_ms{0};
    FuelTankStatus_t _tank_info{};
    uint8_t _transfer_id = 0;
    uint8_t _log_transfer_id = 0;
    // int8_t publish_log();
    DebugLogMessage_t init_mes{};    
    DebugLogMessage_t proc_mes{};    
};


// #ifdef __cplusplus
// }
// #endif

#endif  // SRC_MODULES_FUEL_TANK_HPP_