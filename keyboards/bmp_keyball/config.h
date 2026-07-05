/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#define BMP_BOOTPIN_AS_RESET
#define BMP_DEFAULT_MODE SINGLE
#define ACTION_DEBUG

// pmw3600 configuration
//   parameters for spim.init
#define CONFIG_SPI_FREQ SPI_FREQ_2M
#define CONFIG_MISO_PIN 15
#define CONFIG_MOSI_PIN 14
#define CONFIG_SCK_PIN  16
#define CONFIG_SPI_MODE 3
//   parameters for spi_start
#define CONFIG_SS_PIN 13
#define PMW33XX_CS_PIN CONFIG_SS_PIN

#define POINTING_DEVICE_ROTATION_270
#define POINTING_DEVICE_INVERT_Y
