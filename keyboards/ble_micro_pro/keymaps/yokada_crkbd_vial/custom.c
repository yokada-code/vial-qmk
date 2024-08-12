#include QMK_KEYBOARD_H

#include "custom.h"
#include "oled.h"
#include "wpm.h"
#include "bmp.h"
#include "bmp_indicator_led.h"
#include "state_controller.h"
#include "bmp_custom_keycodes.h"
#include "custom_keycodes.h"
#include "uart_connection.h"

static uint8_t advertise_to_  = 0;
uint8_t get_advertise_to() { return advertise_to_; }
void set_advertise_to(uint8_t id) { advertise_to_ = id; }

// Dummy function always returning 0 to enable oled refresh
int bmp_indicator_user_pattern(uint32_t time_ms, int32_t option) { return 0; }

void ble_slave_user_data_rcv_4byte_cb(bmp_user_data_4byte* dat) {
    switch (dat->type) {
        case BMP_USER_DATA_WPM:
            set_current_wpm(dat->data);
            break;
        default:
            break;
    }
}

bmp_error_t nus_rcv_callback_user(const uint8_t *dat, uint32_t len) {

    dprintf("nus_rcv_callback: received data: length: %u, dat[0]: %u\n", (uint8_t)len, dat[0]);

    if (len == sizeof(rgblight_syncinfo_t)) {
        rgblight_update_sync((rgblight_syncinfo_t *)dat, false);
    } else if (len == sizeof(bmp_user_data_4byte)){
        ble_slave_user_data_rcv_4byte_cb((bmp_user_data_4byte*) dat);
    }

    return BMP_OK;
}

void keyboard_post_init_user(void) {
    bmp_indicator_set(INDICATOR_USER, 0);
    if (!is_keyboard_master()) {
        BMPAPI->ble.set_nus_rcv_cb(nus_rcv_callback_user);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    process_record_user_luna(keycode, record);
    if (record->event.pressed) {
        switch (keycode) {
            case ADV_ID0 ... ADV_ID7:
                set_advertise_to(keycode - ADV_ID0);
                break;
            case AD_WO_L:
                set_advertise_to(255);
                break;
            case UARTINI:
                uart_start_connection();
                break;
            case UARTDIS:
                uart_stop_connection();
                break;
        }
    }

   return true;
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    bool cont = process_record_bmp(keycode, record);

    cont = process_record_user(keycode, record);

    return cont;
}

void bmp_state_change_cb_user(bmp_api_event_t event) {
    update_bt_connection_status_str();
}

void bmp_state_change_cb_kb(bmp_api_event_t event) {
    switch (event) {
        case BLE_CONNECTED:
            is_ble_advertising = false;
            break;
        case BLE_DISCONNECTED:
            is_ble_advertising = false;
            break;
        case BLE_ADVERTISING_START:
            is_ble_advertising = true;
            break;
        case BLE_ADVERTISING_STOP:
            is_ble_advertising = false;
            break;
        default:
            break;
    }
    bmp_state_change_cb_user(event);
}
