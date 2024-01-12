// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_MODULES_FUEL_TANK_HPP_
#define SRC_MODULES_FUEL_TANK_HPP_

#include <stdint.h>
#include "uavcan/equipment/ice/FuelTankStatus.h"

#ifdef __cplusplus
extern "C" {
#endif


class VtolFuelTank {
public:
    VtolFuelTank();
    int8_t init();
    void process();
private:
    void _spin_once();
    uint32_t _last_spin_time_ms{0};
    FuelTankStatus_t _tank_info{};
    uint8_t _transfer_id = 0;
};


#ifdef __cplusplus
}
#endif

#endif  // SRC_MODULES_FUEL_TANK_HPP_