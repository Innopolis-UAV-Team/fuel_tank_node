/*
* Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/

#include "periphery/as5600/as5600.hpp"


using namespace std;
float one_step_angle_val = 0.0;
uint16_t max_raw_value = 4095;

as5600_error_t As5600Periphery::init(uint16_t min_angle, uint16_t max_angle) {
    this->data.raw_angle = 0;
    data.start_angle = min_angle;
    data.max_angle = max_angle;
    one_step_angle_val = (max_angle - min_angle) / (float) max_raw_value;
    return AS5600_SUCCESS;
}

as5600_error_t As5600Periphery::calibrate() {
    return AS5600_SUCCESS;
}

void wait(uint8_t time_ns) {
    uint32_t start_time_ms = HAL_GetTick();
    while (start_time_ms > HAL_GetTick() - time_ns) {
        continue;
    }
}

as5600_error_t As5600Periphery::set_zero_position(uint16_t const a_start_position) {
    return AS5600_SUCCESS;
}

as5600_error_t As5600Periphery::get_magnet_status(uint8_t *const pData) {
    // STATUS register: 00|MD|ML|MH|000
    *pData = 000100;
    return AS5600_SUCCESS;
}

as5600_error_t As5600Periphery::get_angle_data(as5600_addr mem_addr, uint16_t *const pData) {
    uint16_t * raw_angle = &data.raw_angle;
    switch (mem_addr)     {
    case RAW_ANGLE:
        *raw_angle = (HAL_GetTick() / 10) % max_raw_value;
        *pData = *raw_angle;
        printf("AS5600:\tRAW_ANGLE_READING\t%d\n", *pData);
        break;

    case ZPOS:
        printf("AS5600:\tZPOS_READING\t\t%d\n", *pData);
        break;
    case MPOS:
        printf("AS5600:\tMPOS_READING\t\t%d\n", *pData);
        break;
    case CONF:
        printf("AS5600:\tCONF_READING\t\t%d\n", *pData);
        break;
    case STATUS:
        printf("AS5600:\tSTATE_READING\t\t%d\n", *pData);
        break;
    case ANGLE:

        printf("AS5600:\tANGLE_READING\t\t%d\n", *pData);
        break;
    case BURN:
        printf("AS5600:\tBURN_ANGLE_READING\t%d\n", *pData);
        break;
    default:
        printf("AS5600:\tUNKNOW_REGISTER_READING\t%d\n", *pData);
        return AS5600_BAD_PARAMETER;
    }
    return AS5600_SUCCESS;
}
