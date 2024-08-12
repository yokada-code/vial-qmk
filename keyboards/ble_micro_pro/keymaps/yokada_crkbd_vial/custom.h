#pragma once

#include <stdbool.h>
#include <stdint.h>

uint8_t get_advertise_to(void);

typedef struct {
    uint8_t type;
    uint8_t data;
    uint8_t reserved[2];
} bmp_user_data_4byte;

#define BMP_USER_DATA_WPM     1
