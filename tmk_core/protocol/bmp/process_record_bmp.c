// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <stdio.h>

#include "action.h"
#include "bmp.h"
#include "send_string.h"
#include "print.h"
#include "vial.h"

#include "apidef.h"
#include "bmp_custom_keycodes.h"
#include "bmp_host_driver.h"
#include "bmp_settings.h"
#include "state_controller.h"

#define DEFFERED_KEY_RECORD_LEN 6
static keyrecord_t deferred_key_record[DEFFERED_KEY_RECORD_LEN];

static void push_deferred_key_record(uint16_t keycode, keyevent_t *event) {
    for (int i = 0; i < DEFFERED_KEY_RECORD_LEN; i++) {
        if (deferred_key_record[i].keycode == KC_NO) {
            keyrecord_t record     = {.event = *event, .keycode = keycode};
            deferred_key_record[i] = record;
            return;
        }
    }
}

bool process_record_bmp(uint16_t keycode, keyrecord_t* record) {
    // To apply key overrides to keycodes combined shift modifier, separate to two actions
    if (keycode >= QK_MODS && keycode <= QK_MODS_MAX) {
        if (record->event.pressed) {
            register_mods(QK_MODS_GET_MODS(keycode));
            uint16_t   deferred_keycode   = QK_MODS_GET_BASIC_KEYCODE(keycode);
            keyevent_t deferred_key_event = (keyevent_t){.type = KEY_EVENT, .key = (keypos_t){.row = VIAL_MATRIX_MAGIC, .col = VIAL_MATRIX_MAGIC}, .pressed = 1, .time = (timer_read() | 1)};
            push_deferred_key_record(deferred_keycode, &deferred_key_event);
        } else {
            uint16_t   deferred_keycode   = QK_MODS_GET_BASIC_KEYCODE(keycode);
            keyevent_t deferred_key_event = ((keyevent_t){.type = KEY_EVENT, .key = (keypos_t){.row = VIAL_MATRIX_MAGIC, .col = VIAL_MATRIX_MAGIC}, .pressed = 0, .time = (timer_read() | 1)});
            unregister_mods(QK_MODS_GET_MODS(keycode));
            push_deferred_key_record(deferred_keycode, &deferred_key_event);
        }
        return false;
    }

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
                snprintf(str, sizeof(str), "%4dmV", BMPAPI->app.get_vcc_mv(0));
                if (bmp_config->mode == SPLIT_MASTER) {
                    snprintf(str + 6, sizeof(str) - 6, " %4dmV", BMPAPI->app.get_vcc_mv(1));
                }
                send_string(str);
                return false;
            }
            case DISABLE_KEY_OS_OVERRIDE: {
                println("Disable key os overrides");
                bmp_set_key_os_override(BMP_KEY_OS_OVERRIDE_DISABLE);
                return false;
            }
            case ENABLE_US_KEY_ON_JP_OS_OVERRIDE: {
                println(
                    "Perform as an US keyboard on the OS configured for JP");
                bmp_set_key_os_override(BMP_US_KEY_JP_OS_OVERRIDE);
                return false;
            }
            case ENABLE_JP_KEY_ON_US_OS_OVERRIDE: {
                println("Perform as a JP keyboard on the OS configured for US");
                bmp_set_key_os_override(BMP_JP_KEY_US_OS_OVERRIDE);
                return false;
            }
        }
    }
    else {
        switch (keycode) {
            case ENT_SLP: {
                sleep_enter_counter = 1;
                return false;
            }
        }
    }

    return true;
}

void bmp_post_keyboard_task(void) {
    for (int i = 0; i < DEFFERED_KEY_RECORD_LEN; i++) {
        if (deferred_key_record[i].keycode != KC_NO) {
            g_vial_magic_keycode_override = deferred_key_record[i].keycode;
            action_exec(deferred_key_record[i].event);
            deferred_key_record[i].keycode = KC_NO;
        } else {
            return;
        }
    }
}