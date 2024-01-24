/*
* Copyright (C) 2024 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/

#include "periphery/as5600/as5600.hpp"

using namespace std;

// void calc_min_angle_steps(uint32_t min_angle_deg){
//     uint32_t min_angle_steps = ceil(4095 * min_angle_deg/359);
// }

as5600_error_t As5600Periphery::init(uint32_t min_angle, uint32_t max_angle)
{
    _logger.init("as5600");

    as5600_error_t status = AS5600_SUCCESS;

    status = get_magnet_status(&data.mag_status);
    char buffer[90];

    if (status != AS5600_SUCCESS) {
        sprintf(buffer, "GET_MAGN: %d", status);
        _logger.log_error(buffer);
        return status;
    } else {
        sprintf(buffer, "GET_MAGN(val): %d", data.mag_status);
        _logger.log_debug(buffer);
    }
    data.max_value = max_angle;
    data.start_angle = min_angle;
    // data.min_angle_steps = calc_min_angle_steps(min_angle);
    return status;
}

as5600_error_t As5600Periphery::calibrate()
{

    as5600_error_t status = get_angle_data(RAW_ANGLE, &data.raw_angle);
    char buffer[90];

    if (status != AS5600_SUCCESS) {
        sprintf(buffer, "CALIB: %d", status);
        _logger.log_error(buffer);
        return AS5600_I2C_ERROR;
    } else {
        sprintf(buffer, "CALIB(RANG): %d %d", status, data.raw_angle);
        _logger.log_debug(buffer);
    }

    status = set_zero_position(data.raw_angle);
    // i2c_error_t i2c_status = write_16_to_reg(I2C_AS5600, ZPOS, data.raw_angle, AS5600_12_BIT_MASK);
    if (status != AS5600_SUCCESS) {
        sprintf(buffer, "SET_POS: %d", status);
        _logger.log_error(buffer);
        return status;
    }

    wait(100);
    i2c_error_t i2c_status = i2cReadTwoBytesRegister(I2C_AS5600, RAW_ANGLE, &data.raw_angle);
    if (i2c_status != I2C_SUCCESS) {
        sprintf(buffer, "CALIB_1: %d", i2c_status);
        _logger.log_error(buffer);
        return AS5600_I2C_ERROR;
    } else {
        sprintf(buffer, "CALIB_1(RANG): %d %d", status, data.raw_angle);
        _logger.log_debug(buffer);
    }

    wait(100);

    uint8_t const tx_buffer = BURN_ANLGE_VAL;
    size_t const count = sizeof(uint8_t);

    i2c_status = i2cWriteBytesToRegister(I2C_AS5600, BURN, &tx_buffer, count);
    wait(100);

    if (i2c_status != 0) {
        sprintf(buffer, "CALIB_2: %d", i2c_status);
        _logger.log_error(buffer);
        return AS5600_I2C_ERROR;
    } else {
        sprintf(buffer, "CALIB_2(RANG): %d%d", status, data.raw_angle);
        _logger.log_debug(buffer);
    }
    return status;
    // TODO: continue
}

void wait(uint8_t time_ns)
{
    uint32_t start_time_ms = HAL_GetTick();
    while (start_time_ms > HAL_GetTick() - time_ns) {
        continue;
    }
}

as5600_error_t As5600Periphery::set_max_angle(uint16_t const max_angle){

    uint8_t const reg = MPOS;
    uint16_t const reg_mask = 0x0FFFU;
    uint16_t const reg_value = max_angle;

    as5600_error_t status = AS5600_SUCCESS;

    if (reg_mask < reg_value) {
            status = AS5600_BAD_PARAMETER;
    }

    if (AS5600_SUCCESS == status) {
        i2c_error_t i2c_status = i2cWriteTwoBytesToRegister(I2C_AS5600, reg, max_angle);
        if (i2c_status != I2C_SUCCESS){
            status = AS5600_I2C_ERROR;
        }
    }
    return AS5600_SUCCESS;
}

as5600_error_t As5600Periphery::set_zero_position(uint16_t const val)
{
    as5600_error_t status = AS5600_SUCCESS;
    uint16_t const first_byte_mask = 0x00FF;
    size_t const count = sizeof(uint16_t);
    uint8_t tx_buffer[2];

    tx_buffer[0] = (uint8_t)((val >> 8) & first_byte_mask);
    tx_buffer[1] = (uint8_t)(val & first_byte_mask);

    char buffer[90];
    if (status == AS5600_SUCCESS) {
        i2c_error_t i2c_status = i2cWriteBytesToRegister(I2C_AS5600, ZPOS, tx_buffer, count);
        if (i2c_status != I2C_SUCCESS)
        {
            sprintf(buffer, "WRITE: %d", i2c_status);
            _logger.log_error(buffer);
            status = AS5600_I2C_ERROR;
        }
    }
    return status;
}

as5600_error_t As5600Periphery::get_magnet_status(uint8_t *const pData)
{
    char buffer[90];
    as5600_error_t status = AS5600_SUCCESS;

    if (pData == NULL) {
        status = AS5600_BAD_PARAMETER;
        return status;
    }

    i2c_error_t i2c_status = i2cReadByteRegister(I2C_AS5600, STATUS, pData);
    if (i2c_status != I2C_SUCCESS) {
        sprintf(buffer, "GET_STATUS: %d", status);
        _logger.log_error(buffer);
        status = AS5600_I2C_ERROR;
    }
    return status;
}

as5600_error_t As5600Periphery::get_angle_data(as5600_addr mem_addr, uint16_t *const pData)
{

    char buffer[90];
    as5600_error_t status = AS5600_SUCCESS;

    if (pData == NULL) {
        status = AS5600_BAD_PARAMETER;
        return status;
    }
    i2c_error_t i2c_status = i2cReadTwoBytesRegister(I2C_AS5600, mem_addr, pData);

    if (i2c_status != I2C_SUCCESS) {
        sprintf(buffer, "GET_ANGLE_I2C: %d", i2c_status);
        _logger.log_error(buffer);
        status = AS5600_I2C_ERROR;
    }
    return status;
}