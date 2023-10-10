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

/* key matrix size */
#define MATRIX_ROWS 1
#define MATRIX_COLS 19

#define MATRIX_ROW_PINS \
    { 33 }
#define MATRIX_COL_PINS \
    { 1, 2, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22 }

#define DIODE_DIRECTION COL2ROW

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#define RGBLIGHT_SPLIT
#define RGBLED_NUM_DEFAULT 128