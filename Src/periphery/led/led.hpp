// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#ifndef SRC_APPLICATION_PERIPHERY_LED_LED_HPP_
#define SRC_APPLICATION_PERIPHERY_LED_LED_HPP_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum class LedColor {
    RED_COLOR,
    GREEN_COLOR,
    BLUE_COLOR,
    COLORS_AMOUNT,
};

class LedPeriphery {
public:
    static void reset();
    static void toggle(LedColor led_color);
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_APPLICATION_PERIPHERY_LED_LED_HPP_
