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

typedef enum i2c_error_e{
        I2C_SUCCESS = 0,
        I2C_BAD_PARAMETER,
        I2C_RUNTIME_ERROR,
        I2C_TRANSMIT_ERROR,
        I2C_RECEIVE_ERROR,
        I2C_ERROR,
} i2c_error_t;

int8_t isDeviceReady(uint8_t id, uint8_t n_trials);

int8_t i2cTransmit(uint8_t id, const uint8_t tx[], uint8_t len);
int8_t i2cReceive(uint8_t id, uint8_t* rx, uint8_t len);

i2c_error_t get_8_register(uint8_t id, uint8_t mem_addr, uint8_t *const pData);
i2c_error_t get_16_register(uint8_t id, uint8_t mem_addr, uint16_t *const pData);

i2c_error_t write_8_to_reg(uint8_t id, uint8_t mem_addr, uint8_t data);
i2c_error_t write_16_to_reg(uint8_t id, uint8_t mem_addr, uint16_t data, uint16_t bit_mask);


#ifdef __cplusplus
}
#endif

#endif  // SRC_APPLICATION_PERIPHERY_HAL_I2C_