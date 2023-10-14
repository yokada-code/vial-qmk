// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include <stdint.h>

void flash_write_dword(uint32_t addr, uint32_t *data);
void flash_write_page(uint32_t page, uint32_t *data);
void flash_read(uint32_t addr, uint8_t *data, uint32_t len);
void flash_erase_page(uint32_t page);