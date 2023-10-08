// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

// TMK headers
#include "host_driver.h"
#include "host.h"

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

uint8_t keyboard_leds(void) {
    return 0;
}

void send_keyboard(report_keyboard_t *report) {}

void send_mouse(report_mouse_t *report) {}

void send_extra(report_extra_t *report) {}

void raw_hid_send(uint8_t *data, uint8_t length) {}

void protocol_setup(void) {}

void protocol_pre_init(void) {}

void protocol_post_init(void) {
    host_set_driver(&driver);
}

void protocol_pre_task(void) {}

void protocol_post_task(void) {}