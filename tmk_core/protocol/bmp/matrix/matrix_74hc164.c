
#include "quantum.h"

#include "bmp_matrix.h"
#include "apidef.h"
#include "bmp.h"

static void                    init_row2col(void);
static void                    init_col2row(void);
static uint32_t                get_device_row(void);
static uint32_t                get_device_col(void);
static uint32_t                scan_row2col(matrix_row_t *);
static uint32_t                scan_col2row(matrix_row_t *);
extern const bmp_matrix_func_t matrix_func_row2col;
extern const bmp_matrix_func_t matrix_func_col2row;

const bmp_matrix_func_t matrix_func_74hc164row2col = {init_row2col, get_device_row, get_device_col, scan_row2col};
const bmp_matrix_func_t matrix_func_74hc164col2row = {init_col2row, get_device_row, get_device_col, scan_col2row};

static uint32_t get_device_row(void) {
    return bmp_config->matrix.rows;
}

static uint32_t get_device_col(void) {
    return bmp_config->matrix.cols;
}

//
//// col2row matrix
//
static void init_col2row(void) {
    const uint32_t SR_RST = bmp_config->matrix.row_pins[2];
    for (int col = 0; col < bmp_config->matrix.cols; col++) {
        setPinInputHigh(bmp_config->matrix.col_pins[col]);
    }
    writePinHigh(SR_RST);
    setPinOutput(SR_RST);
}

static uint32_t scan_col2row(matrix_row_t *matrix_raw) {
    const uint32_t SR_DATA = bmp_config->matrix.row_pins[0];
    const uint32_t SR_CLK  = bmp_config->matrix.row_pins[1];

    // set initial data
    writePinHigh(SR_DATA);
    for (int row = 0; row < bmp_config->matrix.rows; row++) {
        writePinHigh(SR_CLK);
        writePinLow(SR_CLK);
    }

    writePinLow(SR_DATA);
    writePinHigh(SR_CLK);
    writePinLow(SR_CLK);
    writePinHigh(SR_DATA);

    matrix_row_t current_matrix[MATRIX_ROWS] = {0};

    for (int row = 0; row < bmp_config->matrix.rows; row++) {
        // read row
        for (int col = 0; col < bmp_config->matrix.cols; col++) {
            current_matrix[col] |= (readPin(bmp_config->matrix.col_pins[col]) ? 0 : 1) << row;
        }

        // shift data
        writePinHigh(SR_CLK);
        writePinLow(SR_CLK);
    }

    uint32_t matrix_change = 0;
    for (int row = 0; row < bmp_config->matrix.rows; row++) {
        if (matrix_raw[row] != current_matrix[row]) {
            dprintf("row:%d %04lx\n", row, current_matrix[row]);
            matrix_raw[row] = current_matrix[row];
            matrix_change   = 1;
        }
    }

    return matrix_change;
}

//
//// row2col matrix
//
static void init_row2col(void) {
    const uint32_t SR_RST = bmp_config->matrix.col_pins[2];
    for (int row = 0; row < bmp_config->matrix.rows; row++) {
        setPinInputHigh(bmp_config->matrix.row_pins[row]);
    }
    writePinHigh(SR_RST);
    setPinOutput(SR_RST);
}

static uint32_t scan_row2col(matrix_row_t *matrix_raw) {
    const uint32_t SR_DATA = bmp_config->matrix.col_pins[0];
    const uint32_t SR_CLK  = bmp_config->matrix.col_pins[1];

    // set initial data
    writePinHigh(SR_DATA);
    for (int col = 0; col < bmp_config->matrix.cols; col++) {
        writePinHigh(SR_CLK);
        writePinLow(SR_CLK);
    }
    writePinLow(SR_DATA);
    writePinHigh(SR_CLK);
    writePinLow(SR_CLK);
    writePinHigh(SR_DATA);

    matrix_col_t current_matrix[MATRIX_ROWS] = {0};

    for (int col = 0; col < bmp_config->matrix.cols; col++) {
        // read col
        for (int row = 0; row < bmp_config->matrix.rows; row++) {
            current_matrix[row] |= (readPin(bmp_config->matrix.row_pins[row]) ? 0 : 1) << col;
        }

        // shift data
        writePinHigh(SR_CLK);
        writePinLow(SR_CLK);
    }

    uint32_t matrix_change = 0;
    for (int col = 0; col < bmp_config->matrix.cols; col++) {
        if (matrix_raw[col] != current_matrix[col]) {
            dprintf("col:%d %04lx\n", col, current_matrix[col]);
            matrix_raw[col] = current_matrix[col];
            matrix_change   = 1;
        }
    }

    return matrix_change;
}
