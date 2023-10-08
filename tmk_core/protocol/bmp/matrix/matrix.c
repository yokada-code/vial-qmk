// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "matrix.h"

// TODO: bmp

/* number of matrix rows */
uint8_t matrix_rows(void) {
    return 0;
}

/* number of matrix columns */
uint8_t matrix_cols(void) {
    return 0;
}

/* should be called at early stage of startup before matrix_init.(optional) */
void matrix_setup(void) {}

/* intialize matrix for scaning. */
void matrix_init(void) {}

/* scan all key states on matrix */
uint8_t matrix_scan(void) {
    return 0;
}

/* whether matrix scanning operations should be executed */
bool matrix_can_read(void) {
    return true;
}

/* whether a switch is on */
bool matrix_is_on(uint8_t row, uint8_t col) {
    return false;
}

/* matrix state on row */
matrix_row_t matrix_get_row(uint8_t row) {
    return 0;
}

/* print matrix for debug */
void matrix_print(void) {}