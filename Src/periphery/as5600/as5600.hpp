/*
* Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/

#ifndef PLATFORM_SPECIFIC_AS5600_AS5600_HPP_
#define PLATFORM_SPECIFIC_AS5600_AS5600_HPP_

#include <stdint.h>
#include <string> 
#include "uavcan/protocol/debug/LogMessage.h"
#include "periphery/hal_i2c/hal_i2c.hpp"
#include "logger.hpp"
#include "main.h"
#include <math.h>

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

typedef uint8_t as5600_addr;


typedef enum as5600_error_e
{
        AS5600_SUCCESS = 0,
        AS5600_BAD_PARAMETER,
        AS5600_RUNTIME_ERROR,
        AS5600_I2C_ERROR,
} as5600_error_t;


struct as5600_data
{
    bool dir; // 0 clockwise | 1 counterclockwise 
    uint16_t raw_angle;
    uint16_t max_angle;
    uint16_t start_angle;
    uint16_t angle;
    uint8_t mag_status;
    uint8_t min_angle_deg = 18;
};

void wait(uint8_t time_ns);

class As5600Periphery
{
public:
    // TODO: add sensor parameters
    as5600_data data = {.dir=0,.raw_angle = 0, .max_angle = 4, .start_angle=0, .angle=400, .mag_status=0};
    
    as5600_error_t init(uint16_t min_angle, uint16_t max_angle);
    int8_t reset();
    int8_t update_zpos();
    as5600_error_t calibrate();
    
    as5600_error_t get_angle_data(as5600_addr mem_addr, uint16_t *const pData);
    as5600_error_t get_magnet_status(uint8_t *const pData);
    as5600_error_t set_zero_position(uint16_t const a_start_position);
    as5600_error_t set_max_angle(uint16_t const max_angle);
private:
    void calc_min_angle_steps();
    Logger _logger{};
    uint8_t _log_transfer_id = 0;
};

#endif // PLATFORM_SPECIFIC_AS5600_AS5600_HPP_
