// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

// QMK headers
#include "util.h"
#include "print.h"
#include "matrix.h"
#include "raw_hid.h"
#include "keyboard.h"
#include "rgblight.h"

// TMK headers
#include "host_driver.h"
#include "host.h"

// BMP headers
#include "apidef.h"
#include "cli.h"
#include "bmp.h"
#include "state_controller.h"
#include "bmp_vial.h"
#include "bmp_indicator_led.h"

#ifndef MATRIX_SCAN_TIME_MS
#    define MATRIX_SCAN_TIME_MS 17
#endif
const uint8_t MAINTASK_INTERVAL       = MATRIX_SCAN_TIME_MS;

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
const bmp_api_config_t *bmp_config;

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
                                                 .layer           = 4,
                                                 .debounce        = 1,
                                                 .diode_direction = DIODE_DIRECTION == ROW2COL ? 1 : 0,
                                                 .row_pins        = MATRIX_ROW_PINS,
                                                 .col_pins        = MATRIX_COL_PINS,
                                             },
                                         .encoder = {
                                            .pin_a = ENCODERS_PAD_A,
                                            .pin_b = ENCODERS_PAD_B,
                                         },
                                         .param_peripheral = {60, 30, 7},
                                         .param_central    = {60, 30, 7},
                                         .led              = {.pin = WS2812_DI_PIN, .num = 1},
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

bmp_error_t nus_rcv_callback(const uint8_t *dat, uint32_t len) {
    if (len == sizeof(rgblight_syncinfo_t)) {
        rgblight_update_sync((rgblight_syncinfo_t *)dat, false);
    }
    return BMP_OK;
}

int bmp_validate_config(const bmp_api_config_t *config) {
    if (config->version != CONFIG_VERSION                        //
        || config->matrix.rows > 32 || config->matrix.cols > 32  //
        || config->matrix.device_rows > config->matrix.rows      //
        || config->matrix.device_cols > config->matrix.cols      //
        || config->matrix.layer > 32 || config->matrix.layer < 1 //
        || config->mode >= WEBNUS_CONFIG                         //
        || config->led.num > RGBLED_NUM                          //

    ) {
        return 1;
    }
    return 0;
}

void bmp_init(void) {
    if (BMPAPI->api_version != API_VERSION) {
        BMPAPI->bootloader_jump();
    }

    BMPAPI->logger.init();
    BMPAPI->logger.info("logger init");

    is_safe_mode_ = (BMPAPI->app.init() > 0);

    bmp_vial_data_init();
    bmp_config = &flash_vial_data.bmp_config;
    if (bmp_validate_config(bmp_config) || BMPAPI->app.set_config(bmp_config)) {
        bmp_config = &default_config;
        BMPAPI->app.set_config(bmp_config);
    }

    BMPAPI->usb.set_msc_write_cb(msc_write_callback);
    BMPAPI->app.set_state_change_cb(bmp_state_change_cb);
    BMPAPI->usb.set_raw_receive_cb(bmp_raw_hid_receive);
    if (bmp_config->mode == SPLIT_SLAVE) {
        BMPAPI->ble.set_nus_rcv_cb(nus_rcv_callback);
    }

    BMPAPI->usb.init(bmp_config, false);
    BMPAPI->ble.init(bmp_config);
    BMPAPI->logger.info("usb init");
    cli_init();

    BMPAPI->usb.enable();
    BMPAPI->logger.info("usb enable");
}

static bool do_keyboard_task = false;
void        bmp_main_task(void *_) {
    do_keyboard_task = true;
    // bmp_indicator_task(MAINTASK_INTERVAL);
}

void protocol_setup(void) {
    bmp_init();
    host_set_driver(&driver);
}

void protocol_pre_init(void) {
    bmp_dynamic_keymap_init();
}

void protocol_post_init(void) {
    rgblight_set_clipping_range(0, bmp_config->led.num);
    rgblight_set_effect_range(0, bmp_config->led.num);
    bmp_indicator_init(bmp_config->reserved[1]);

    print_set_sendchar((sendchar_func_t)BMPAPI->usb.serial_putc);
    BMPAPI->app.main_task_start(bmp_main_task, MAINTASK_INTERVAL);
}

void protocol_pre_task(void) {
    BMPAPI->app.process_task();
}

void protocol_post_task(void) {
    // if (bmp_config->mode == SPLIT_MASTER && rgblight_get_change_flags()) {
    //     static rgblight_syncinfo_t rgblight_sync;
    //     rgblight_get_syncinfo(&rgblight_sync);
    //     BMPAPI->ble.nus_send_bytes((uint8_t *)&rgblight_sync, sizeof(rgblight_sync));
    //     rgblight_clear_change_flags();
    // }

    BMPAPI->usb.process();
    cli_exec();
    bmp_mode_transition_check();
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
