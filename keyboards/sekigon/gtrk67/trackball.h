
#pragma once

#include <stdint.h>

typedef struct {
    uint8_t stat;
    int8_t x;
    int8_t y;
} trackball_data_t;

int trackball_init(uint8_t tb_cs, uint8_t init1, uint8_t init2);
int trackball_enable(uint8_t tb_cs);
trackball_data_t trackball_get(uint8_t tb_cs);
uint8_t trackball_get_status(uint8_t tb_cs);

int trackball_sleep(uint8_t tb_cs);
int trackball_wakeup(uint8_t tb_cs);
