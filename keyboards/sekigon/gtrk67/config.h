// Copyright 2024 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#ifndef __ASSEMBLER__

#define CONFIG_SCK_PIN 9
#define CONFIG_MISO_PIN 7
#define CONFIG_MOSI_PIN 10
#define CONFIG_SS_PIN 8
#define CONFIG_SPI_FREQ SPI_FREQ_2M
#define CONFIG_SPI_MODE 3

#define MATRIX_ROWS_DEFAULT 5
#define MATRIX_COLS_DEFAULT 15
#define MATRIX_SCAN_TIME_MS 20
#define ACTION_DEBUG

#define BMP_DEFAULT_MODE 0
#define THIS_DEVICE_ROWS MATRIX_ROWS_DEFAULT
#define THIS_DEVICE_COLS MATRIX_COLS_DEFAULT
#define BMP_DIODE_DIRECTION MATRIX_74HC164COL
#define BMP_STARTUP_ADVERTISE 1
#define BMP_USE_DEFAULT_VIAL_CONFIG

#define ENCODERS_PAD_A {}
#define ENCODERS_PAD_B {}

#define CONFIG_RESERVED {0, 2, 0, 0, 0, 0, 0, 0}

#    include "microshell/util/mscmd.h"
// #    define USER_DEFINED_MSCMD

#endif