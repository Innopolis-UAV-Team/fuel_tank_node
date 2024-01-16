// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_APPLICATION_PERIPHERY_AS5600_AS5600_HPP_
#define SRC_APPLICATION_PERIPHERY_AS5600_AS5600_HPP_

#include <stdint.h>
#include "i2c.h"
#include <string> 
#include "uavcan/protocol/debug/LogMessage.h"

/**
 * @note AS5600 registers
 */
#define ZPOS        0x01        //
#define MPOS        0x03        //
#define CONF        0x07        //
#define RAW_ANGLE   0x0C        // 
#define ANGLE       0x0E        // has the limit of the 360 degree range
#define STATE       0x0B        // gives MH, ML and MD
#define BURN_ANGLE  0xFF        // use to permanently program the device.

#define I2C_ADDRESS_AS5600  0x36
#define I2C_TIMOUT_MS       1000
#define AS5600_12_BIT_MASK (uint16_t)4095

#define AS5600_MAGNET_DETECTED                                                 \
    (uint8_t)(1UL << 5) /*Status bit indicates b-field is detected */

typedef uint8_t as5600_addr;

struct as5600_data
{
    /* data */
    uint16_t raw_angle;
    uint16_t max_value;
    uint16_t start_angle;
    uint8_t mag_status;
};

class As5600Periphery
{
public:
    // TODO: add sensor parameters
    as5600_data data = {.raw_angle = 0, .max_value = 4, .start_angle=0, .mag_status=0};
    // as5600_data data = {.raw_angle = 0, .max_value = 4095, .start_angle=0,};
    
    HAL_StatusTypeDef init();
    int8_t reset();
    int8_t update_zpos();
    HAL_StatusTypeDef get_angle_data(as5600_addr mem_addr, uint16_t *const pData);
    HAL_StatusTypeDef get_magnet_status(uint8_t *const stat);
    // int8_t get_data(as5600_addr mem_addr, uint16_t pData, int n_bytes);
    HAL_StatusTypeDef write_data(as5600_addr mem_addr, uint16_t pData, int n_bytes);
private:
    HAL_StatusTypeDef get_8_register(as5600_addr mem_addr, uint8_t  *const pData);
    HAL_StatusTypeDef get_16_register(as5600_addr mem_addr, uint16_t  *const pData);
    HAL_StatusTypeDef write_16_to_reg(as5600_addr mem_addr, uint16_t *const data);
    HAL_StatusTypeDef write_8_to_reg(as5600_addr mem_addr, uint8_t *const data);


    uint8_t _log_transfer_id = 0;

    DebugLogMessage_t init_mes{};    
    DebugLogMessage_t proc_mes{};    
};

#endif // SRC_APPLICATION_PERIPHERY_AS5600_AS5600_HPP_
