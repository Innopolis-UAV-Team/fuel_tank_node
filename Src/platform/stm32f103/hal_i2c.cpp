/*
 * Copyright (C) 2018-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 
 * Original file: https://github.com/ZilantRobotics/libperiph/blob/6f5fa8331cd32ba9d4f1363946f0ff0f611f80f9/platform_specific/hal_i2c/stm32/hal_i2c.c
 */

#include "periphery/hal_i2c/hal_i2c.hpp"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_I2C_MODULE_ENABLED
    extern I2C_HandleTypeDef hi2c1;
#endif

#define I2C_TIMEOUT_MS          100


HAL_StatusTypeDef i2cTransmit(uint8_t id, const uint8_t tx[], uint8_t len) {
#ifdef HAL_I2C_MODULE_ENABLED
    HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(&hi2c1, id, (uint8_t*)tx, len, I2C_TIMEOUT_MS);
    if (res == HAL_OK) {
        return res;
    }
#else
    UNUSED(id);
    UNUSED(tx);
    UNUSED(len);
#endif
    return HAL_ERROR;
}

HAL_StatusTypeDef i2cReceive(uint8_t id, uint8_t* rx, uint8_t len) {
#ifdef HAL_I2C_MODULE_ENABLED
    HAL_StatusTypeDef res = HAL_I2C_Master_Receive(&hi2c1, id, rx, len, I2C_TIMEOUT_MS);
    if (res == HAL_OK) {
        return res;
    }
#else
    UNUSED(id);
    UNUSED(rx);
    UNUSED(len);
#endif
    return HAL_ERROR;
}
#ifdef __cplusplus
}
#endif