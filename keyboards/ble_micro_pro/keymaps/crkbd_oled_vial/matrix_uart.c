#include QMK_KEYBOARD_H
#include "bmp_matrix.h"
#include "bmp_debounce.h"
#include "uart_connection.h"

#include <stdbool.h>

typedef struct {
    uint8_t count;
    bmp_api_key_event_t key_state[16];
} bmp_uart_keystate_t;

extern const uint8_t MAINTASK_INTERVAL;
static bmp_uart_keystate_t uart_buf;

#define DEFAULT_MATRIX_ROWS 32
static matrix_row_t matrix_dummy[DEFAULT_MATRIX_ROWS];
static matrix_row_t matrix_debouncing[DEFAULT_MATRIX_ROWS];
static const bmp_matrix_func_t *matrix_func;
extern const bmp_matrix_func_t  matrix_func_col2row;

static void bmp_uart_cb(uint8_t* data, uint8_t len) {
    memcpy(&uart_buf, data, len);
}

void matrix_init_user(void) {
    // initialize matrix state: all keys off
    for (uint8_t i = 0; i < DEFAULT_MATRIX_ROWS; i++) {
        matrix_dummy[i] = 0;
        matrix_debouncing[i] = 0;
    }

    matrix_func = &matrix_func_col2row;
    uart_set_cb(UART_TYPE_KC_SYNC, bmp_uart_cb);
    uart_init();
}

uint8_t matrix_scan_impl(matrix_row_t *_matrix) {
    const bmp_api_config_t *config     = BMPAPI->app.get_config();
    const uint8_t           device_row = matrix_func->get_device_row();
    const uint8_t           device_col = matrix_func->get_device_col();
    uint8_t                 matrix_offset =
        config->matrix.is_left_hand ? 0 : config->matrix.rows - device_row;
    int matrix_changed = 0;

    uint32_t raw_changed = matrix_func->scan(matrix_debouncing);
    bmp_api_key_event_t key_state[16];
    matrix_changed = bmp_debounce(
        matrix_debouncing + matrix_offset, matrix_dummy + matrix_offset,
        device_row, device_col,
        config->matrix.debounce * MAINTASK_INTERVAL, raw_changed, key_state);

    if (is_uart_established()) {
        for (int i = 0; i < matrix_changed; i++) {
            key_state[i].row += matrix_offset;
        }

        if (!is_keyboard_master() && matrix_changed > 0) { //SLAVE
            uart_buf.count = matrix_changed;
            memcpy(uart_buf.key_state, key_state,
                    sizeof(bmp_api_key_event_t) * matrix_changed);
            uart_send(UART_TYPE_KC_SYNC, (uint8_t *)&uart_buf,
                    sizeof(bmp_api_key_event_t) * matrix_changed + 1);
        }
    } else {
        for (int i = 0; i < matrix_changed; i++) {
            BMPAPI->app.push_keystate_change(&key_state[i]);
        }
    }

    if (debug_config.keyboard && matrix_changed > 0) {
        dprintf("device rows:\n");
        for (uint8_t idx = 0; idx < device_row; idx++) {
            if (device_col <= 8) {
                dprintf("\tdr%02d:0x%02x\n", idx,
                        (uint8_t)matrix_debouncing[idx + matrix_offset]);
            } else if (device_col <= 16) {
                dprintf("\tdr%02d:0x%04x\n", idx,
                        (uint8_t)matrix_debouncing[idx + matrix_offset]);
            } else {
                dprintf("\tdr%02d:0x%08x\n", idx,
                        (uint8_t)matrix_debouncing[idx + matrix_offset]);
            }
        }
        dprintf("\n");
    }

    uint32_t pop_cnt = 0;
    if (is_uart_established()) {
        if (is_keyboard_master()) { //MASTER
            pop_cnt = matrix_changed + uart_buf.count;
            memcpy(&key_state[matrix_changed], uart_buf.key_state,
                   sizeof(bmp_api_key_event_t) * uart_buf.count);
            uart_buf.count = 0;
        }
    } else {
        pop_cnt = BMPAPI->app.pop_keystate_change(
            key_state, sizeof(key_state) / sizeof(key_state[0]),
            config->param_central.max_interval / MAINTASK_INTERVAL + 3);
    }

    for (uint32_t i = 0; i < pop_cnt; i++) {
        if (key_state[i].state == 0) {
            _matrix[key_state[i].row] &= ~(1 << key_state[i].col);
        } else {
            _matrix[key_state[i].row] |= (1 << key_state[i].col);
        }
    }

    if (debug_config.keyboard && pop_cnt > 0) {
        dprintf("matrix rows:\n");
        for (uint8_t idx = 0; idx < config->matrix.rows; idx++) {
            if (config->matrix.cols <= 8) {
                dprintf("\tr%02d:0x%02x\n", idx, (uint8_t)_matrix[idx]);
            } else if (config->matrix.cols <= 16) {
                dprintf("\tr%02d:0x%04x\n", idx, (uint8_t)_matrix[idx]);
            } else {
                dprintf("\tr%02d:0x%08x\n", idx, (uint8_t)_matrix[idx]);
            }
        }
        dprintf("\n");
    }

    return pop_cnt;
}
