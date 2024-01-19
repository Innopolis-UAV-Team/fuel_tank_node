/*
 * Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
 * Distributed under the terms of the GPL v3 license, available in the file
 */

#ifndef PLATFORM_SPECIFIC_HAL_I2C_HPP
#define PLATFORM_SPECIFIC_HAL_I2C_HPP

#include <cstdint>
// #include <stdint.h>
#include <cstddef>


#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum i2c_error_e {
  I2C_SUCCESS = 0,
  I2C_BAD_PARAMETER,
  I2C_RUNTIME_ERROR,
  I2C_TRANSMIT_ERROR,
  I2C_RECEIVE_ERROR,
  I2C_ERROR,
} i2c_error_t;

int8_t isDeviceReady(uint8_t id, uint8_t n_trials);

int8_t i2cTransmit(uint8_t id, const uint8_t tx[], uint8_t len);
int8_t i2cReceive(uint8_t id, uint8_t *rx, uint8_t len);

i2c_error_t get_8_register(uint8_t id, uint8_t mem_addr, uint8_t *const pData);
i2c_error_t get_16_register(uint8_t id, uint8_t mem_addr,
                            uint16_t *const pData);

i2c_error_t write_n_consecutive_bytes(uint8_t id, uint8_t reg,
                                      uint8_t const *const p_tx,
                                      size_t n_bytes);
#ifdef __cplusplus
}
#endif

#endif  // PLATFORM_SPECIFIC_HAL_I2C_HPP