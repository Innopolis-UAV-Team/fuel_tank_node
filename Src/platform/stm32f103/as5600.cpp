// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#include "periphery/as5600/as5600.hpp"
#include "main.h"
#include <math.h>


#include <string> 
#include "i2c.h"

using namespace std;


int8_t As5600Periphery::init() {

    uint8_t hal_status = HAL_OK;

    uint8_t check, select;

    float zpos = 0.0;
    float angle = 0.0;
    uint16_t raw_angle = 0;
    uint16_t little_endian_raw_angle = 0;

    // Confirm device
    hal_status = HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS_AS5600, RAW_ANGLE, 2, (uint8_t *) &raw_angle, 2, I2C_TIMOUT_MS);
    
    // change the big endian bytes from the as5600 to little endian data format
    little_endian_raw_angle = __builtin_bswap16(raw_angle);
    

    if (HAL_OK != hal_status){
        return hal_status;
    };
    // cout<<little_endian_raw_angle<<endl;

    this->data.raw_angle = little_endian_raw_angle;

    return 0;
}


int8_t As5600Periphery::get_data(as5600_addr mem_addr, uint16_t pData, int n_bytes){
    
    // HAL_StatusTypeDef hal_status = HAL_I2C_Master_Receive(&hi2c1, I2C_ADDRESS_AS5600, mem_addr, (uint8_t *) &pData, 2, I2C_TIMOUT_MS);
    HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS_AS5600, mem_addr, n_bytes, (uint8_t *) &pData, n_bytes, I2C_TIMOUT_MS);
    
    // change the big endian bytes from the as5600 to little endian data format
    uint16_t pData_lit = __builtin_bswap16(pData);
    pData = pData_lit;


    if (HAL_OK != hal_status){
        return hal_status;
    };

    return 0;
}

int8_t As5600Periphery::write_data(as5600_addr mem_addr, uint16_t pData, int n_bytes){

    // change the little endian bytes to the big endian data format for as5600 
    uint16_t pData_big = __builtin_bswap16(pData);
    pData = pData_big;


    HAL_StatusTypeDef hal_status = HAL_I2C_Mem_Write(&hi2c1, I2C_ADDRESS_AS5600, mem_addr, n_bytes, (uint8_t *) &pData, n_bytes, I2C_TIMOUT_MS);

    return hal_status;
}