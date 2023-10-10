// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

// QMK headers
#include "util.h"
#include "print.h"
#include "matrix.h"
#include "raw_hid.h"
#include "keyboard.h"

// TMK headers
#include "host_driver.h"
#include "host.h"

// BMP headers
#include "apidef.h"
#include "cli.h"

#ifndef MATRIX_SCAN_TIME_MS
#    define MATRIX_SCAN_TIME_MS 17
#endif
const uint8_t MAINTASK_INTERVAL = MATRIX_SCAN_TIME_MS;
int           reset_counter;

/* -------------------------
 *   TMK host driver defs
 * -------------------------
 */

/* declarations */
uint8_t keyboard_leds(void);
void    send_keyboard(report_keyboard_t *report);
void    send_mouse(report_mouse_t *report);
void    send_extra(report_extra_t *report);

/* host struct */
host_driver_t driver = {keyboard_leds, send_keyboard, send_mouse, send_extra};

// TODO: bmp

const bmp_api_config_t default_config = {.version     = CONFIG_VERSION,
                                         .mode        = BMP_DEFAULT_MODE == 0   ? SINGLE
                                                        : BMP_DEFAULT_MODE == 1 ? SPLIT_MASTER
                                                        : BMP_DEFAULT_MODE == 2 ? SPLIT_SLAVE
                                                                                : SINGLE,
                                         .device_info = {PRODUCT_ID, VENDOR_ID, PRODUCT, STR(MANUFACTURER)},
                                         .matrix =
                                             {
                                                 .rows            = MATRIX_ROWS_DEFAULT,
                                                 .cols            = MATRIX_COLS_DEFAULT,
                                                 .device_rows     = THIS_DEVICE_ROWS,
                                                 .device_cols     = THIS_DEVICE_COLS,
                                                 .debounce        = 1,
                                                 .diode_direction = DIODE_DIRECTION == ROW2COL ? 1 : 0,
                                                 .row_pins        = MATRIX_ROW_PINS,
                                                 .col_pins        = MATRIX_COL_PINS,
                                             },
                                         .param_peripheral = {60, 30, 7},
                                         .param_central    = {60, 30, 7},
                                         .led              = {.pin = WS2812_DI_PIN, .num = RGBLED_NUM_DEFAULT},
#ifdef CONFIG_RESERVED
                                         .reserved = CONFIG_RESERVED
#endif
};

static bool is_safe_mode_ = false;

bool is_safe_mode(void) {
    return is_safe_mode_;
}

bmp_error_t msc_write_callback(const uint8_t *dat, uint32_t len) {
    return BMP_OK;
}

bmp_error_t bmp_state_change_cb(bmp_api_event_t event) {
    return BMP_OK;
}

void bmp_raw_hid_receive(const uint8_t *data, uint8_t len) {
    static uint8_t via_data[32];
    if (len > sizeof(via_data) + 1) {
        printf("<raw_hid>Too large packet");
        return;
    }

    memcpy(via_data, data, len - 1);

    raw_hid_receive(via_data, len - 1);
}

void bmp_init(void) {
    if (BMPAPI->api_version != API_VERSION) {
        BMPAPI->bootloader_jump();
    }

    BMPAPI->logger.init();
    BMPAPI->logger.info("logger init");

    const bmp_api_config_t *config = &default_config;
    is_safe_mode_                  = (BMPAPI->app.init(&default_config) > 0);

    // start in safe mode
    BMPAPI->app.set_config(&default_config);

    BMPAPI->usb.set_msc_write_cb(msc_write_callback);
    BMPAPI->app.set_state_change_cb(bmp_state_change_cb);
    BMPAPI->usb.set_raw_receive_cb(bmp_raw_hid_receive);

    BMPAPI->usb.init(config, true);
    BMPAPI->ble.init(config);
    BMPAPI->logger.info("usb init");
    cli_init();

    BMPAPI->usb.enable();
    BMPAPI->logger.info("usb enable");
}

static bool do_keyboard_task = false;
void        bmp_main_task(void *_) {
    do_keyboard_task = true;
}

void protocol_setup(void) {
    bmp_init();
    host_set_driver(&driver);
}

void protocol_pre_init(void) {}

void protocol_post_init(void) {
    print_set_sendchar((sendchar_func_t)BMPAPI->usb.serial_putc);
    BMPAPI->app.main_task_start(bmp_main_task, MAINTASK_INTERVAL);
}

void protocol_pre_task(void) {
    BMPAPI->app.process_task();
}

void protocol_post_task(void) {
    BMPAPI->usb.process();
    cli_exec();
}

void protocol_task(void) {
    protocol_pre_task();

    if(do_keyboard_task)
    {
        do_keyboard_task = false;
        keyboard_task();
    }

    protocol_post_task();
}

bool is_keyboard_master(void) {
    return BMPAPI->app.get_config()->mode != SPLIT_SLAVE;
}

bool is_keyboard_left(void) {
    return BMPAPI->app.get_config()->matrix.is_left_hand;
}
