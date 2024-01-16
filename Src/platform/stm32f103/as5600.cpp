// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#include "periphery/as5600/as5600.hpp"
#include "main.h"
#include <math.h>
#include "logging.h"



using namespace std;


HAL_StatusTypeDef As5600Periphery::init() {
    // TODO: check is it nedded to change the big endian bytes from the as5600 to little endian data format

    set_source(this->init_mes, "as5600_init");
    set_source(this->proc_mes, "as5600_proc");

    HAL_StatusTypeDef hal_status = HAL_OK;


    hal_status = get_16_register(RAW_ANGLE, &data.raw_angle);
    if (hal_status != HAL_OK){
        char buffer[90];
        sprintf(buffer, "AS5600 get_16_reg status: %d", hal_status);
        set_text(this->init_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
        _log_transfer_id++;
        return hal_status;
    }

    hal_status = get_magnet_status(&data.mag_status);
    if (hal_status != HAL_OK){
        char buffer[90];
        sprintf(buffer, "AS5600 get_magnet status: %d %d", hal_status, data.mag_status);
        set_text(this->init_mes, buffer);
        dronecan_protocol_debug_log_message_publish(&init_mes, &_log_transfer_id);
        _log_transfer_id++;
        return hal_status;
    }

    
    return hal_status;
}

HAL_StatusTypeDef As5600Periphery::get_magnet_status(uint8_t *const pData) {

    return get_8_register(STATE, pData);
}

HAL_StatusTypeDef As5600Periphery::write_16_to_reg(as5600_addr mem_addr, uint16_t *const data){
    uint8_t container[2] = {0};
    container[0] = (uint8_t)((*data & AS5600_12_BIT_MASK) >>
                        8); /* Zero out upper four bits of
                                argument and shift out lower four
                                bits */
    container[1] = (uint8_t) *data;
    return HAL_I2C_Mem_Write_IT(&hi2c1, I2C_ADDRESS_AS5600, mem_addr, I2C_MEMADD_SIZE_8BIT, container, 2);
}


HAL_StatusTypeDef As5600Periphery::write_8_to_reg(as5600_addr mem_addr, uint8_t *const data){
    return HAL_I2C_Mem_Write_IT(&hi2c1, I2C_ADDRESS_AS5600, mem_addr, I2C_MEMADD_SIZE_8BIT, data, 1);
}


HAL_StatusTypeDef As5600Periphery::get_16_register(as5600_addr mem_addr, uint16_t  *const pData){
    uint8_t data[2] = {0};
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read_IT(&hi2c1, I2C_ADDRESS_AS5600,
                            mem_addr,
                            I2C_MEMADD_SIZE_8BIT, data, 2);

    *pData = ((data[0] << 8) | data[1]);
    return status;
}

HAL_StatusTypeDef As5600Periphery::get_8_register(as5600_addr mem_addr, uint8_t  *const pData){
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read_IT(&hi2c1, I2C_ADDRESS_AS5600,
                            mem_addr,
                            I2C_MEMADD_SIZE_8BIT, pData, 1);
    return status;
}

HAL_StatusTypeDef As5600Periphery::get_angle_data(as5600_addr mem_addr, uint16_t *const pData){
    
    HAL_StatusTypeDef hal_status = get_16_register(mem_addr, pData);
    
    // change the big endian bytes from the as5600 to little endian data format
    // uint16_t pData_lit = __builtin_bswap16(pData);
    // pData = pData_lit;

    return hal_status;
}

HAL_StatusTypeDef As5600Periphery::write_data(as5600_addr mem_addr, uint16_t pData, int n_bytes){

    // change the little endian bytes to the big endian data format for as5600 
    uint16_t pData_big = __builtin_bswap16(pData);
    pData = pData_big;


    HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Write(&hi2c1, I2C_ADDRESS_AS5600, mem_addr, n_bytes, (uint8_t *) &pData, n_bytes, I2C_TIMOUT_MS);

    return hal_status;
}