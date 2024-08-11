#include QMK_KEYBOARD_H

#include "custom.h"
#include "oled.h"
#include "bmp.h"
#include "apidef.h"

uint8_t display_flags = 0;
bool is_ble_advertising = false;
uint8_t ble_con_status[CON_STATUS_STR_LEN];
uint8_t ble_con_hostname[CON_HOSTNAME_LEN];

uint8_t rgblight_mode_name[RGBLIGHT_MODE_NAME_LEN];

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    } else {
        return OLED_ROTATION_0;
    }
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        print_status_luna();
    } else {
        print_status_bongo();
    }
    return true;
}

char get_bt_advertisement_status_char(void){
    if (is_ble_advertising){
        uint8_t advertise_to = get_advertise_to();
        if (advertise_to == 255) {
            return 'A';
        }
        return '0' + (char) advertise_to;
    }
    return '-';
}

void update_rgblight_mode_name(void) {}
