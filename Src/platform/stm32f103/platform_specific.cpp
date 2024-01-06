// Copyright (C) 2023 Dmitry Ponomarev <ponomarevda96@gmail.com>
// Distributed under the terms of the GPL v3 license, available in the file LICENSE.

#include <string.h>
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void uavcanReadUniqueID(uint8_t out_uid[4]) {
    const uint32_t UNIQUE_ID_16_BYTES[4] = {
        HAL_GetUIDw0(),
        HAL_GetUIDw1(),
        HAL_GetUIDw2(),
        0
    };
    memcpy(out_uid, UNIQUE_ID_16_BYTES, 16);
}

void uavcanRestartNode() {
    HAL_NVIC_SystemReset();
}

uint32_t uavcanGetTimeMs() {
    return HAL_GetTick();
}

#ifdef __cplusplus
}
#endif
