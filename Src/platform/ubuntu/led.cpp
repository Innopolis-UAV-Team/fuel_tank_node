// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#include "periphery/led/led.hpp"

void LedPeriphery::reset() {
}

void LedPeriphery::toggle(LedColor led_color) {
    (void)led_color;
}
