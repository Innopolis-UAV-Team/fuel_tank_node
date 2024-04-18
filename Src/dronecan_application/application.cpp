// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#include "application.hpp"
#include "dronecan.h"
#include "main.h"
#include "params.hpp"
#include "storage.h"
#include "periphery/led/led.hpp"
#include "modules/fuel_tank.hpp"

#ifdef HAL_IWDG_MODULE_ENABLED
extern IWDG_HandleTypeDef hiwdg;
#endif

void application_entry_point() {
    paramsInit((ParamIndex_t)IntParamsIndexes::INTEGER_PARAMS_AMOUNT, NUM_OF_STR_PARAMS, -1, 1);
    paramsLoad();

    auto node_id = paramsGetIntegerValue(PARAM_UAVCAN_NODE_ID);
    auto tank_id = paramsGetIntegerValue(PARAM_FUEL_TANK_ID);

    uavcanInitApplication(node_id);
    uavcanSetNodeName("arl.fuel_tank");

    LedPeriphery::reset();

    VtolFuelTank fuel_tank;
    fuel_tank.init(tank_id, paramsGetIntegerValue(PARAM_FUEL_TANK_FULL) , paramsGetIntegerValue(PARAM_FUEL_TANK_EMPTY), paramsGetIntegerValue(PARAM_FUEL_TANK_VOLUME));    
    while(true) {
        if (fuel_tank.module_status != ModuleStatus::MODULE_OK) {

            LedPeriphery::toggle(LedColor::RED_COLOR);
        } else {
            LedPeriphery::toggle(LedColor::BLUE_COLOR);
        }
        uavcanSetNodeHealth((NodeStatusHealth_t)fuel_tank.module_status);
        fuel_tank.process();
        uavcanSpinOnce();

        #ifdef HAL_IWDG_MODULE_ENABLED
            HAL_IWDG_Refresh(&hiwdg);
        #endif
    }
}
