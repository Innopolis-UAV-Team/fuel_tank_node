// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_APPLICATION_PERIPHERY_AS5600_AS5600_HPP_
#define SRC_APPLICATION_PERIPHERY_AS5600_AS5600_HPP_

#include <stdint.h>

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

typedef uint8_t as5600_addr;

struct as5600_data
{
    /* data */
    float raw_angle;
    float max_value;
};

class As5600Periphery
{
public:
    // TODO: add sensor parameters
    as5600_data data = {.raw_angle = 0.0, .max_value = 1.0};

    int8_t init();
    int8_t reset();
    int8_t update_zpos();
    int8_t get_data(as5600_addr mem_addr, uint16_t pData, int n_bytes);
    int8_t write_data(as5600_addr mem_addr, uint16_t pData, int n_bytes);
private:
    uint32_t _last_spin_time_ms{0};
};

#endif // SRC_APPLICATION_PERIPHERY_AS5600_AS5600_HPP_
