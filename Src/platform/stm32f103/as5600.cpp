// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

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

    as5600_error_t status = AS5600_ERROR_SUCCESS;

    status = get_magnet_status(&data.mag_status);
    if (status != AS5600_ERROR_SUCCESS) {
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
    if (status != AS5600_ERROR_SUCCESS) {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate status: %d", status);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
        return status;
    } else {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate status: %d RAW_ANGLE value: %d", status, data.raw_angle);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
    }

    status = write_16_to_reg(ZPOS, data.raw_angle);
    if (status != AS5600_ERROR_SUCCESS) {
        return status;
    }

    wait(100);
    status = get_16_register(RAW_ANGLE, &data.raw_angle);
    if (status != AS5600_ERROR_SUCCESS) {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate 1 step ERROR hal_status: %d", status);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
        return status;
    } else {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate 1 step status: %d RAW_ANGLE value: %d", status, data.raw_angle);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
    }

    wait(100);

    status = write_8_to_reg(BURN, BURN_ANLGE_VAL);
    wait(100);

    if (status != AS5600_ERROR_SUCCESS) {
        char buffer[90];
        sprintf(buffer, "AS5600 calibrate 2 step ERROR hal_status: %d", status);
        set_text(this->calb_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&calb_mes, &_log_transfer_id);
        _log_transfer_id++;
        return status;
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
    as5600_error_t status = AS5600_ERROR_SUCCESS;
    if (a_start_position == NULL) {
        status = AS5600_ERROR_BAD_PARAMETER;
        return status;
    }

    if (status == AS5600_ERROR_SUCCESS) {
        status = write_16_to_reg(ZPOS, a_start_position);
        if (status != AS5600_ERROR_SUCCESS)
        {
            char buffer[90];
            sprintf(buffer, "get_angle_data HAL_STATUS %d", status);
            set_text(this->proc_mes, buffer);
            dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
            _log_transfer_id++;
        }
    }
    return status;
}

as5600_error_t As5600Periphery::get_magnet_status(uint8_t *const pData)
{
    as5600_error_t status = AS5600_ERROR_SUCCESS;

    if (pData == NULL) {
        status = AS5600_ERROR_BAD_PARAMETER;
        return status;
    }

    status = get_8_register(STATUS, pData);
    if (status != AS5600_ERROR_SUCCESS) {
        char buffer[90];
        sprintf(buffer, "get_magn_data HAL_STATUS %d", status);
        set_text(this->proc_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
        _log_transfer_id++;
    }
    return status;
}

as5600_error_t As5600Periphery::get_angle_data(as5600_addr mem_addr, uint16_t *const pData)
{

    as5600_error_t status = AS5600_ERROR_SUCCESS;

    if (pData == NULL) {
        status = AS5600_ERROR_BAD_PARAMETER;
        return status;
    }
    status = get_16_register(mem_addr, pData);

    if (status != AS5600_ERROR_SUCCESS) {
        char buffer[90];
        sprintf(buffer, "get_angle_data ERROR %d", status);
        set_text(this->proc_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&proc_mes, &_log_transfer_id);
        _log_transfer_id++;
    }
    return status;
}

as5600_error_t As5600Periphery::write_16_to_reg(as5600_addr mem_addr, uint16_t data)
{
    HAL_StatusTypeDef hal_status = HAL_OK;

    uint8_t container[2] = {0};
    container[0] = (uint8_t)((data & AS5600_12_BIT_MASK) >>
                             8); /* Zero out upper four bits of
                                     argument and shift out lower four
                                     bits */
    container[1] = (uint8_t)data;
    hal_status = HAL_I2C_Mem_Write_IT(&hi2c1, I2C_ADDRESS_AS5600, mem_addr, I2C_MEMADD_SIZE_8BIT, container, I2C_MEMADD_SIZE_16BIT);
    if (hal_status != HAL_OK){
        return AS5600_ERROR_I2C_RECEIVE_ERROR;
    }
    
    return AS5600_ERROR_SUCCESS;
}

as5600_error_t As5600Periphery::write_8_to_reg(as5600_addr mem_addr, uint8_t data)
{
    HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Write_IT(&hi2c1, I2C_ADDRESS_AS5600, mem_addr, I2C_MEMADD_SIZE_8BIT, &data, I2C_MEMADD_SIZE_8BIT);
    if (hal_status != HAL_OK){
        return AS5600_ERROR_I2C_RECEIVE_ERROR;
    }

    return AS5600_ERROR_SUCCESS;
}

as5600_error_t As5600Periphery::get_16_register(as5600_addr mem_addr, uint16_t *const pData)
{

    uint8_t tx_buf[I2C_MEMADD_SIZE_8BIT] = {0};
    tx_buf[0] = mem_addr;
    uint8_t data[2] = {0x00};
    // HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Read(&hi2c1, (I2C_ADDRESS_AS5600 << 1), mem_addr, I2C_MEMADD_SIZE_16BIT, data, I2C_MEMADD_SIZE_16BIT, HAL_MAX_DELAY);

    // HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Read_IT(&hi2c1, I2C_ADDRESS_AS5600<<1,
    //                         mem_addr,
    //                         I2C_MEMADD_SIZE_8BIT, data, I2C_MEMADD_SIZE_8BIT);

    HAL_StatusTypeDef hal_status = i2cTransmit(I2C_AS5600, tx_buf, 1);
    if (hal_status != HAL_OK) {
        return AS5600_ERROR_I2C_TRANSMIT_ERROR;
    }
    hal_status = i2cReceive(I2C_AS5600, data, 2);
    // change the big endian bytes from the as5600 to little endian data format
    // uint16_t pData_lit = __builtin_bswap16(pData);
    // pData = pData_lit;
    // HAL_StatusTypeDef hal_status = i2cReceive(mem_addr<<1, data, I2C_MEMADD_SIZE_16BIT);
    if (hal_status!=HAL_OK){
        return AS5600_ERROR_I2C_RECEIVE_ERROR;
    }

    *pData = (((uint16_t)data[0] << 8) | (uint16_t)data[1]);
    return AS5600_ERROR_SUCCESS;
}

as5600_error_t As5600Periphery::get_8_register(as5600_addr mem_addr, uint8_t *const pData)
{
    // HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Read_IT(&hi2c1, I2C_ADDRESS_AS5600<<1,
    //                         mem_addr,
    //                         I2C_MEMADD_SIZE_8BIT, pData, 1);
    uint8_t tx_buf[I2C_MEMADD_SIZE_8BIT] = {0x00};
    tx_buf[0] = mem_addr;
    HAL_StatusTypeDef hal_status = i2cTransmit(I2C_AS5600, tx_buf, I2C_MEMADD_SIZE_8BIT);
    if (hal_status != HAL_OK) {
        return AS5600_ERROR_I2C_TRANSMIT_ERROR;
    }

    hal_status = i2cReceive(I2C_AS5600, pData, I2C_MEMADD_SIZE_8BIT);
    if (hal_status!=HAL_OK){
        return AS5600_ERROR_I2C_RECEIVE_ERROR;
    }
    return AS5600_ERROR_SUCCESS;
}