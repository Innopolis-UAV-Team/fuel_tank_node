/*
 * Copyright (C) 2018-2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
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


int8_t i2cTransmit(uint8_t id, const uint8_t tx[], uint8_t len) {
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

int8_t i2cReceive(uint8_t id, uint8_t* rx, uint8_t len) {
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

int8_t isDeviceReady(uint8_t id, uint8_t n_trials){
    HAL_StatusTypeDef hal_status = HAL_I2C_IsDeviceReady(&hi2c1, id, n_trials, I2C_TIMEOUT_MS);
    return hal_status;
}

/**
  * @brief Reads 16 bits from specified register of the i2c device
  * @param id Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param mem_addr Target device register
  * @param pData Pointer to data buffer
  * @retval i2c_error_t status
*/
i2c_error_t get_16_register(uint8_t id, uint8_t mem_addr, uint16_t *const pData)
{

    uint8_t tx_buf[I2C_MEMADD_SIZE_8BIT] = {0};
    tx_buf[0] = mem_addr;
    uint8_t data[2] = {0x00};

    int8_t hal_status = i2cTransmit(id, tx_buf, 1);
    if (hal_status != 0) {
        return I2C_TRANSMIT_ERROR;
    }
    hal_status = i2cReceive(id, data, 2);
    if (hal_status != HAL_OK){
        return I2C_RECEIVE_ERROR;
    }

    *pData = (((uint16_t)data[0] << 8) | (uint16_t)data[1]);
    return I2C_SUCCESS;
}

/**
  * @brief Reads 8 bits from specified register of the i2c device
  * @param id Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param mem_addr Target device register
  * @param pData Pointer to data buffer
  * @retval i2c_error_t status
*/
i2c_error_t get_8_register(uint8_t id, uint8_t mem_addr, uint8_t *const pData)
{
    uint8_t tx_buf[I2C_MEMADD_SIZE_8BIT] = {0x00};
    tx_buf[0] = mem_addr;
    int8_t hal_status = i2cTransmit(id, tx_buf, I2C_MEMADD_SIZE_8BIT);
    if (hal_status != 0) {
        return I2C_TRANSMIT_ERROR;
    }

    hal_status = i2cReceive(id, pData, I2C_MEMADD_SIZE_8BIT);
    if (hal_status != 0){
        return I2C_RECEIVE_ERROR;
    }
    return I2C_SUCCESS;
}

i2c_error_t write_16_to_reg(uint8_t id, uint8_t mem_addr, uint16_t data, uint16_t bit_mask=65535)
{
    HAL_StatusTypeDef hal_status = HAL_OK;

    uint8_t container[2] = {0};
    container[0] = (uint8_t)((data & bit_mask) >>
                             8); /* Zero out upper four bits of
                                     argument and shift out lower four
                                     bits */
    container[1] = (uint8_t)data;
    hal_status = HAL_I2C_Mem_Write_IT(&hi2c1, id, mem_addr, I2C_MEMADD_SIZE_8BIT, container, I2C_MEMADD_SIZE_16BIT);
    if (hal_status != HAL_OK){
        return I2C_ERROR;
    }
    return I2C_SUCCESS;
}

i2c_error_t write_8_to_reg(uint8_t id, uint8_t mem_addr, uint8_t data)
{
    HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Write_IT(&hi2c1, id, mem_addr, I2C_MEMADD_SIZE_8BIT, &data, I2C_MEMADD_SIZE_8BIT);
    if (hal_status != HAL_OK){
        return I2C_ERROR;
    }
    return I2C_SUCCESS;
}

#ifdef __cplusplus
}
#endif