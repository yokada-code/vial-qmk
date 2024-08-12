#include QMK_KEYBOARD_H

#include "custom.h"
#include "oled.h"
#include "wpm.h"
#include "bmp_indicator_led.h"

uint8_t get_advertise_to(void) {
    return 255;  //not implemented
}

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
