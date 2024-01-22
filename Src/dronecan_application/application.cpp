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
    paramsInit(static_cast<uint8_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT), NUM_OF_STR_PARAMS);
    paramsLoadFromFlash();

    auto node_id = paramsGetIntegerValue(PARAM_UAVCAN_NODE_ID);
    auto node_name = (const char*)paramsGetStringValue(static_cast<uint8_t>(IntParamsIndexes::INTEGER_PARAMS_AMOUNT));

    LedPeriphery::reset();
    uavcanInitApplication(node_id);
    uavcanSetNodeName(node_name);
    // full - 173
    // empty - 235
    int8_t res = 0;
    VtolFuelTank fuel_tank;
    res = fuel_tank.init(0, 173, 235, 5);    
    while(true) {
        if (res != 0){
            LedPeriphery::toggle(LedColor::RED_COLOR);
        }
        else{
        LedPeriphery::toggle(LedColor::BLUE_COLOR);
        }
        res = fuel_tank.process();
        uavcanSpinOnce();

        #ifdef HAL_IWDG_MODULE_ENABLED
            HAL_IWDG_Refresh(&hiwdg);
        #endif
    }
}
