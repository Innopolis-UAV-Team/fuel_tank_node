// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#include "periphery/led/led.hpp"
#include "main.h"


void LedPeriphery::reset() {
    HAL_GPIO_WritePin(INT_RGB_LED_BLUE_GPIO_Port, INT_RGB_LED_BLUE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(INT_RGB_LED_GREEN_GPIO_Port, INT_RGB_LED_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(INT_RGB_LED_RED_GPIO_Port, INT_RGB_LED_RED_Pin, GPIO_PIN_SET);
}

void LedPeriphery::toggle(LedColor led_color) {
    auto crnt_time_ms = HAL_GetTick();
    GPIO_PinState state = (crnt_time_ms % 1000 > 500) ? GPIO_PIN_SET : GPIO_PIN_RESET;

    switch (led_color) {
        case LedColor::RED_COLOR:
            HAL_GPIO_WritePin(INT_RGB_LED_RED_GPIO_Port, INT_RGB_LED_RED_Pin, state);
            HAL_GPIO_WritePin(INT_RGB_LED_GREEN_GPIO_Port, INT_RGB_LED_GREEN_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(INT_RGB_LED_BLUE_GPIO_Port, INT_RGB_LED_BLUE_Pin, GPIO_PIN_SET);
            break;

        case LedColor::GREEN_COLOR:
            HAL_GPIO_WritePin(INT_RGB_LED_RED_GPIO_Port, INT_RGB_LED_RED_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(INT_RGB_LED_GREEN_GPIO_Port, INT_RGB_LED_GREEN_Pin, state);
            HAL_GPIO_WritePin(INT_RGB_LED_BLUE_GPIO_Port, INT_RGB_LED_BLUE_Pin, GPIO_PIN_SET);
            break;

        case LedColor::BLUE_COLOR:
            HAL_GPIO_WritePin(INT_RGB_LED_RED_GPIO_Port, INT_RGB_LED_RED_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(INT_RGB_LED_GREEN_GPIO_Port, INT_RGB_LED_GREEN_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(INT_RGB_LED_BLUE_GPIO_Port, INT_RGB_LED_BLUE_Pin, state);
            break;

        default:
            break;
    }
}
