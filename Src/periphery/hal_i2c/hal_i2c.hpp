/*
 * Copyright (C) 2018-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 
 * Original file: https://github.com/ZilantRobotics/libperiph/blob/6f5fa8331cd32ba9d4f1363946f0ff0f611f80f9/platform_specific/hal_i2c/hal_i2c.h
 */
 
#ifndef SRC_APPLICATION_PERIPHERY_HAL_I2C_
#define SRC_APPLICATION_PERIPHERY_HAL_I2C_

#include "main.h"


#ifdef __cplusplus
extern "C" {
#endif

HAL_StatusTypeDef i2cTransmit(uint8_t id, const uint8_t tx[], uint8_t len);
HAL_StatusTypeDef i2cReceive(uint8_t id, uint8_t* rx, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif  // SRC_APPLICATION_PERIPHERY_HAL_I2C_