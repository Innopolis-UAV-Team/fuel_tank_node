// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#include "periphery/as5600/as5600.hpp"
#include "main.h"
#include <math.h>


#include <string> 
#include <stdio.h>
using namespace std;

// HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
// HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)


//   HAL_I2C_Master_Transmit(&hi2c1, (I2C_ADDRESS << 1), &regAddress, 1,  I2C_TIMEOUT);
//   HAL_I2C_Master_Receive(&hi2c1, (I2C_ADDRESS << 1), &regData, 1,  I2C_TIMEOUT);


int8_t As5600Periphery::init() {
    this->data.raw_angle = 0;
    return 0;
}


int8_t As5600Periphery::get_angle_data(as5600_addr mem_addr, uint16_t *const pData){
    
    switch (mem_addr)
    {
    case RAW_ANGLE:
        this->data.raw_angle+= 0.1;
        *pData = this->data.raw_angle;
        printf("AS5600:\tRAW_ANGLE_READING\t%d\n", pData);
        break;

    case ZPOS:
        printf("AS5600:\tZPOS_READING\t\t%d\n", pData);
        break;
    case MPOS:
        printf("AS5600:\tMPOS_READING\t\t%d\n", pData);
        break;
    case CONF:
        printf("AS5600:\tCONF_READING\t\t%d\n", pData);
        break;
    case STATE:
        printf("AS5600:\tSTATE_READING\t\t%d\n", pData);
        break;
    case ANGLE:
        printf("AS5600:\tANGLE_READING\t\t%d\n", pData);
        break;
    case BURN_ANGLE:
        printf("AS5600:\tBURN_ANGLE_READING\t%d\n", pData);
        break;
    default:
        printf("AS5600:\tUNKNOW_REGISTER_READING\t%d\n", pData);
        return 1;
    }
    return 0;
}

// void PwmPeriphery::set_duration(const PwmPin pwm_pin, uint32_t duration_us) {
//     switch (pwm_pin) {
//         case PwmPin::PWM_1:
//             TIM4->CCR2 = duration_us;
//             break;

//         case PwmPin::PWM_2:
//             TIM4->CCR1 = duration_us;
//             break;

//         case PwmPin::PWM_3:
//             TIM3->CCR1 = duration_us;
//             break;

//         case PwmPin::PWM_4:
//             TIM3->CCR2 = duration_us;
//             break;

//         default:
//             break;
//     }
// }

// uint32_t PwmPeriphery::get_duration(PwmPin pwm_pin) {
//     uint32_t pwm_duration;

//     switch (pwm_pin) {
//         case PwmPin::PWM_1:
//             pwm_duration = TIM4->CCR2;
//             break;

//         case PwmPin::PWM_2:
//             pwm_duration = TIM4->CCR1;
//             break;

//         case PwmPin::PWM_3:
//             pwm_duration = TIM3->CCR1;
//             break;

//         case PwmPin::PWM_4:
//             pwm_duration = TIM3->CCR2;
//             break;

//         default:
//             pwm_duration = 0;
//             break;
//     }

//     return pwm_duration;
// }
