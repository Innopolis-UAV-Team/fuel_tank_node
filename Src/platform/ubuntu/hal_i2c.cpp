/*
 * Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 * Distributed under the terms of the GPL v3 license
 */

#include "periphery/hal_i2c/hal_i2c.hpp"
#include <stdint.h>
#include <cstddef>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_I2C_MODULE_ENABLED
    extern I2C_HandleTypeDef hi2c1;
#endif

#define I2C_TIMEOUT_MS          100

int8_t isDeviceReady(uint8_t id, uint8_t n_trials){
    return 0;
}

int8_t i2cTransmit(uint8_t id, const uint8_t tx[], uint8_t len){
    return 0;
}
int8_t i2cReceive(uint8_t id, uint8_t* rx, uint8_t len){
    if (len = 1){
        *rx = 255;
    } else if (len == 2){
        rx[0] = 255;
        rx[1] = 255;
    } else {
        for(std::size_t i=0; i < sizeof(rx) / sizeof(uint8_t); i++){
            rx[i] = 255;
        }
    }
    return 0;
}

i2c_error_t get_8_register(uint8_t id, uint8_t mem_addr, uint8_t *const pData){
    uint8_t tx_buf[1] = {0x00};

    int8_t status = i2cTransmit(id, tx_buf, 1);
    status = i2cReceive(id, pData, 1);
    return (i2c_error_t) status;
}
i2c_error_t get_16_register(uint8_t id, uint8_t mem_addr, uint16_t *const pData){

    uint8_t tx_buf[1] = {mem_addr};
    uint8_t data[2] = {0x00};

    int8_t hal_status = i2cTransmit(id, tx_buf, 1);
    if (hal_status != 0) {
        return I2C_TRANSMIT_ERROR;
    }
    hal_status = i2cReceive(id, data, 2);
    if (hal_status != 0){
        return I2C_RECEIVE_ERROR;
    }

    *pData = (((uint16_t)data[0] << 8) | (uint16_t)data[1]);
    return I2C_SUCCESS;
}

i2c_error_t write_8_to_reg(uint8_t id, uint8_t mem_addr, uint8_t data){
    return I2C_SUCCESS;
}

i2c_error_t write_16_to_reg(uint8_t id, uint8_t mem_addr, uint16_t data, uint16_t bit_mask){
    return I2C_SUCCESS;
}

#ifdef __cplusplus
}
#endif