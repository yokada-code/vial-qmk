/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#define BMP_BOOTPIN_AS_RESET
/* key matrix size */
#define MATRIX_ROWS_DEFAULT 1
#define MATRIX_COLS_DEFAULT 19
#define THIS_DEVICE_ROWS 1
#define THIS_DEVICE_COLS 19
#define IS_LEFT_HAND  true
#define BMP_DEFAULT_MODE SINGLE
#define ACTION_DEBUG
/* key matrix size */

#define DIODE_DIRECTION COL2ROW

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#define RGBLIGHT_SPLIT
#define RGBLED_NUM_DEFAULT 128