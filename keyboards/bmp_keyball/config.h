/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#define BMP_BOOTPIN_AS_RESET

#define IS_LEFT_HAND  true
#define BMP_DEFAULT_MODE SINGLE
#define ACTION_DEBUG

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

// Keyball specific configuration
#define CONFIG_SCK_PIN  16
#define CONFIG_MISO_PIN 15
#define CONFIG_MOSI_PIN 14
#define PMW3360_NCS_PIN 13
#define CONFIG_SPI_FREQ SPI_FREQ_2M
#define CONFIG_SPI_MODE 3
