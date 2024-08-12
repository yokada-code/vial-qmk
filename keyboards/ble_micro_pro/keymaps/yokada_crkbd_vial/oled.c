#include QMK_KEYBOARD_H

#include "custom.h"
#include "oled.h"
#include "bmp.h"
#include "apidef.h"
#include "bmp_host_driver.h"

uint8_t display_flags = 0;
bool is_ble_advertising = false;
uint8_t ble_con_status[CON_STATUS_STR_LEN];
uint8_t ble_con_hostname[CON_HOSTNAME_LEN];

uint8_t rgblight_mode_name[RGBLIGHT_MODE_NAME_LEN];

static char get_hex_char(uint8_t i){
    if (i<10) {
        return '0' + (char) i;
    } else if (i<16) {
        return 'a' + (char) i-10;
    } else {
        return '!';
    }
}

void send_wpm(void) {
    static uint8_t last_wpm = 0;
    uint8_t current_wpm = get_current_wpm();
    if (last_wpm != current_wpm) {
        bmp_user_data_4byte dat = {
            .type = BMP_USER_DATA_WPM,
            .data = current_wpm,
        };
        BMPAPI->ble.nus_send_bytes((uint8_t*)&dat, sizeof(dat));
        dprintf("send_wpm: sent data: length: %u, dat[0]: %u\n", sizeof(dat), ((uint8_t*)&dat)[0]);
        last_wpm = current_wpm;
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (is_keyboard_master()) {
        return OLED_ROTATION_270;
    } else {
        return OLED_ROTATION_0;
    }
}

static uint32_t periodic_task_oled_master_timer = 0;
void periodic_task_oled_master(void) {
    if (timer_elapsed32(periodic_task_oled_master_timer) > 100) {
        static uint16_t old_stat = 0;
        uint16_t stat = BMPAPI->ble.get_connection_status();
        if (old_stat != stat){
            update_bt_connection_status_str();
        }
        send_wpm();
        periodic_task_oled_master_timer = timer_read32();
    }
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        periodic_task_oled_master();
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

void update_bt_connection_status_str(void){
    bmp_api_bonding_info_t peers[8];
    uint32_t peer_cnt = sizeof(peers)/sizeof(peers[0]);
    uint8_t bonding_map = 0;

    BMPAPI->ble.get_bonding_info(peers, &peer_cnt);
    for (int i = 0; i < peer_cnt; i++) {
         bonding_map |= 1 << peers[i].id;
    }

    ble_con_status[0] = ' ';
    if (is_keyboard_master()) {
        if (get_ble_enabled()){
            uint8_t *hostname_full = NULL;
            uint16_t stat = BMPAPI->ble.get_connection_status();
            int i;

            if ((stat >> 8) == 0) {
                ble_con_status[1] = '-';
                strcpy((char*)ble_con_hostname, "----");
            } else {
                ble_con_status[1] = get_hex_char(stat & 0xff);

                for (i = 0; i < peer_cnt; i++) {
                     if (peers[i].id == (stat & 0xff)) {
                         hostname_full = peers[i].name;
                         break;
                     }
                }

                // Using memcpy to avoid buffer overrun and
                // chop the string with the display length.
                memcpy(ble_con_hostname, hostname_full, CON_HOSTNAME_LEN);
                ble_con_hostname[CON_HOSTNAME_LEN-1] = '\0';
            }
        } else if (get_usb_enabled()){
            ble_con_status[1] = '-';
            strcpy((char*)ble_con_hostname, " USB");
        } else {
            ble_con_status[1] = '!';
            strcpy((char*)ble_con_hostname, " ERR");
        }
    } else {
        ble_con_status[1] = 'M';
        ble_con_hostname[0] = '\0';
    }
    ble_con_status[2] = ':';
    ble_con_status[3] = get_hex_char(bonding_map >> 4);
    ble_con_status[4] = get_hex_char(bonding_map & 0xf);
    ble_con_status[5] = '\0';
}

void update_rgblight_mode_name(void) {}
