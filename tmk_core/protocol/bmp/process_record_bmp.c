// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <stdio.h>

#include "action.h"
#include "send_string.h"

#include "apidef.h"
#include "bmp_custom_keycodes.h"
#include "bmp_host_driver.h"
#include "state_controller.h"

bool process_record_bmp(uint16_t keycode, keyrecord_t* record) {
    if (record->event.pressed) {
        switch (keycode) {
            case SEL_BLE:
                select_ble();
                return false;
            case SEL_USB:
                select_usb();
                return false;
            case ADV_ID0 ... ADV_ID7:
                BMPAPI->ble.advertise(keycode - ADV_ID0);
                return false;
            case AD_WO_L:
                BMPAPI->ble.advertise(255);
                return false;
            case DEL_ID0 ... DEL_ID7:
                BMPAPI->ble.delete_bond(keycode - DEL_ID0);
                return false;
            case DELBNDS:
                BMPAPI->ble.delete_bond(255);
                return false;
            case BATT_LV: {
                char str[16];
                snprintf(str, sizeof(str), "%4dmV", BMPAPI->app.get_vcc_mv());
                send_string(str);
                return false;
            }
        }
    }
    else {
        switch (keycode) {
            case ENT_SLP: {
                sleep_enter_counter = 10;
                return false;
            }
        }
    }

    return true;
}
