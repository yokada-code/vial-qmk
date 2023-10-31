/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "ble_micro_pro.h"

#include "bmp.h"

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    bool cont = process_record_bmp(keycode, record);

    if (cont) {
        process_record_user(keycode, record);
    }

    return cont;
}

__attribute__((weak)) void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id = &(data[0]);
    if (*command_id == id_set_keyboard_value) {
        // This version does not handle save flag
    } else {
        *command_id = id_unhandled;
    }
}