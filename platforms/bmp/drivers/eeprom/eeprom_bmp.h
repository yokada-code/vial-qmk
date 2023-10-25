// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

typedef enum {
    EEPROM_BMP_CACHE_WRITE_BACK,
    EEPROM_BMP_CACHE_WRITE_THROUGH,
} EEPROM_BMP_CACHE_MODE;

void eeprom_bmp_flush(void);
void eeprom_bmp_set_cache_mode(EEPROM_BMP_CACHE_MODE mode);
int  eeprom_bmp_load_default(void);
void eeprom_bmp_save_default(void);