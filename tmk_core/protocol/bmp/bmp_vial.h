// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#include <stdint.h>

void bmp_vial_data_init(void);
void bmp_raw_hid_receive(const uint8_t *data, uint8_t len);