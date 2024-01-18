// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_APPLICATION_PERIPHERY_AS5600_AS5600_HPP_
#define SRC_APPLICATION_PERIPHERY_AS5600_AS5600_HPP_

#include <stdint.h>
// #include "i2c.h"
#include <string> 
#include "uavcan/protocol/debug/LogMessage.h"
#include "periphery/hal_i2c/hal_i2c.hpp"

/**
 * @note AS5600 registers
 */
#define ZPOS        0x01        //
#define MPOS        0x03        //
#define MANG        0x05        // max angle
#define CONF        0x07        //
#define RAW_ANGLE   0x0C        // 
#define ANGLE       0x0E        // has the limit of the 360 degree range
#define STATUS      0x0B        // gives MH, ML and MD
#define BURN  0xFF        // use to permanently program the device.

#define BURN_ANLGE_VAL      0x80
#define I2C_ADDRESS_AS5600  0x36 
#define I2C_AS5600          (I2C_ADDRESS_AS5600 << 1) + 1
#define I2C_TIMOUT_MS       1000
#define AS5600_12_BIT_MASK (uint16_t)4095

#define AS5600_MAGNET_DETECTED                                                 \
    (uint8_t)(1UL << 5) /*Status bit indicates b-field is detected */

#define I2C_REQUEST_SIZE    1
#define I2C_RESPONSE_SIZE   2

static uint8_t AS5600_tx_buf[I2C_REQUEST_SIZE]  = {0x00};
static uint8_t AS5600_rx_buf[I2C_RESPONSE_SIZE] = {0x00};


typedef uint8_t as5600_addr;


typedef enum as5600_error_e
{
        AS5600_ERROR_SUCCESS = 0,
        AS5600_ERROR_BAD_PARAMETER,
        AS5600_ERROR_RUNTIME_ERROR,
        AS5600_ERROR_I2C_TRANSMIT_ERROR,
        AS5600_ERROR_I2C_RECEIVE_ERROR,
        AS5600_ERROR_NOT_INITIALIZED,
        AS5600_ERROR_MAGNET_NOT_DETECTED,
        AS5600_ERROR_MAX_WRITE_CYCLES_REACHED,
        AS5600_ERROR_MIN_ANGLE_TOO_SMALL,
        AS5600_ERROR_GENERAL_ERROR,
        AS5600_ERROR_COUNT
} as5600_error_t;


struct as5600_data
{
    /* data */
    uint16_t raw_angle;
    uint16_t max_value;
    uint16_t start_angle;
    uint8_t mag_status;
};

void wait(uint8_t time_ns);

class As5600Periphery
{
public:
    // TODO: add sensor parameters
    as5600_data data = {.raw_angle = 0, .max_value = 4, .start_angle=0, .mag_status=0};
    // as5600_data data = {.raw_angle = 0, .max_value = 4095, .start_angle=0,};
    
    as5600_error_t init();
    int8_t reset();
    int8_t update_zpos();
    as5600_error_t calibrate();
    
    as5600_error_t get_angle_data(as5600_addr mem_addr, uint16_t *const pData);
    as5600_error_t get_magnet_status(uint8_t *const stat);
    // int8_t get_data(as5600_addr mem_addr, uint16_t pData, int n_bytes);
    as5600_error_t set_zero_position(uint16_t const a_start_position);

    as5600_error_t write_data(as5600_addr mem_addr, uint16_t pData, int n_bytes);
private:
    as5600_error_t get_8_register(as5600_addr mem_addr, uint8_t  *const pData);
    as5600_error_t get_16_register(as5600_addr mem_addr, uint16_t  *const pData);
    as5600_error_t write_16_to_reg(as5600_addr mem_addr, uint16_t data);
    as5600_error_t write_8_to_reg(as5600_addr mem_addr, uint8_t data);


    uint8_t _log_transfer_id = 0;

    DebugLogMessage_t init_mes{};    
    DebugLogMessage_t proc_mes{};    
    DebugLogMessage_t calb_mes{};    
};

#endif // SRC_APPLICATION_PERIPHERY_AS5600_AS5600_HPP_
