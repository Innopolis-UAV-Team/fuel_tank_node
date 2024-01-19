/*
* Copyright (C) 2023 Anastasiia Stepanova <asiiapine@gmail.com>
* Distributed under the terms of the GPL v3 license, available in the file LICENSE.
*/

#include "periphery/as5600/as5600.hpp"
#include "main.h"
#include <math.h>
#include "logging.h"

using namespace std;

as5600_error_t As5600Periphery::init()
{
    // TODO: check is it nedded to change the big endian bytes from the as5600 to little endian data format

    set_source(this->init_mes, "as5600_init");
    set_source(this->proc_mes, "as5600_proc");
    set_source(this->calb_mes, "as5600_calb");

    as5600_error_t status = AS5600_SUCCESS;

    status = get_magnet_status(&data.mag_status);
    if (status != AS5600_SUCCESS) {
        char buffer[90];
        sprintf(buffer, "ERROR AS5600 get_magnet(AS5600_STAT): %d", status);
        set_text(this->init_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
        _log_transfer_id++;
        return status;
    } else {
        char buffer[90];
        sprintf(buffer, "AS5600 get_magnet STATE_reg): %d", data.mag_status);
        set_text(this->init_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
        _log_transfer_id++;
    }

    return status;
}

as5600_error_t As5600Periphery::calibrate()
{

    as5600_error_t status = get_angle_data(RAW_ANGLE, &data.raw_angle);
    if (status != AS5600_SUCCESS) {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate status: %d", status);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
        return AS5600_I2C_ERROR;
    } else {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate status: %d RAW_ANGLE value: %d", status, data.raw_angle);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
    }

    i2c_error_t i2c_status = write_16_to_reg(I2C_AS5600, ZPOS, data.raw_angle, AS5600_12_BIT_MASK);
    if (i2c_status != I2C_SUCCESS) {
        return AS5600_I2C_ERROR;
    }

    wait(100);
    i2c_status = get_16_register(I2C_AS5600, RAW_ANGLE, &data.raw_angle);
    if (i2c_status != I2C_SUCCESS) {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate 1 step ERROR i2c_status: %d", i2c_status);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
        return AS5600_I2C_ERROR;
    } else {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate 1 step status: %d RAW_ANGLE value: %d", status, data.raw_angle);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
    }

    wait(100);

    i2c_status = write_8_to_reg(I2C_AS5600, BURN, BURN_ANLGE_VAL);
    wait(100);

    if (i2c_status != 0) {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate 2 step ERROR hal_status: %d", i2c_status);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
        return AS5600_I2C_ERROR;
    } else {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate 2 step status (HAL_STAT, RANG_reg): %d%d", status, data.raw_angle);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
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

as5600_error_t As5600Periphery::set_zero_position(uint16_t const a_start_position)
{
    as5600_error_t status = AS5600_SUCCESS;

    if (status == AS5600_SUCCESS) {
        i2c_error_t i2c_status = write_16_to_reg(I2C_AS5600, ZPOS, a_start_position, AS5600_12_BIT_MASK);
        if (i2c_status != I2C_SUCCESS)
        {
            char buffer[90];
            sprintf(buffer, "get_angle_data I2C_STATUS %d", i2c_status);
            set_text(this->proc_mes, buffer);
            dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
            _log_transfer_id++;
            status = AS5600_I2C_ERROR;
        }
    }
    return status;
}

as5600_error_t As5600Periphery::get_magnet_status(uint8_t *const pData)
{
    as5600_error_t status = AS5600_SUCCESS;

    if (pData == NULL) {
        status = AS5600_BAD_PARAMETER;
        return status;
    }

    i2c_error_t i2c_status = get_8_register(I2C_AS5600, STATUS, pData);
    if (i2c_status != I2C_SUCCESS) {
        char buffer[90];
        sprintf(buffer, "get_magn_data HAL_STATUS %d", status);
        set_text(this->proc_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
        _log_transfer_id++;
        status = AS5600_I2C_ERROR;
    }
    return status;
}

as5600_error_t As5600Periphery::get_angle_data(as5600_addr mem_addr, uint16_t *const pData)
{

    as5600_error_t status = AS5600_SUCCESS;

    if (pData == NULL) {
        status = AS5600_BAD_PARAMETER;
        return status;
    }
    i2c_error_t i2c_status = get_16_register(I2C_AS5600, mem_addr, pData);

    if (i2c_status != I2C_SUCCESS) {
        char buffer[90];
        sprintf(buffer, "get_angle_data ERROR %d", status);
        set_text(this->proc_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
        _log_transfer_id++;
    }
    return status;
}