// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include <stdint.h>

#include "apidef.h"

typedef struct {
    uint32_t         len;
    uint8_t          data[BMP_USER_FLASH_PAGE_SIZE - sizeof(uint32_t) - sizeof(bmp_api_config_t) - 64];
    bmp_api_config_t bmp_config;
    uint8_t          reserved[64];
} flash_vial_data_t;

extern flash_vial_data_t flash_vial_data;

void bmp_vial_data_init(void);
void bmp_raw_hid_receive(const uint8_t *data, uint8_t len);
void bmp_vial_save_config(void);