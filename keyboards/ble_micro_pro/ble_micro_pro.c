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
