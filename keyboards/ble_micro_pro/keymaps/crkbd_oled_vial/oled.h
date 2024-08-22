#pragma once

#include "wpm.h"
#include "custom_keycodes.h"

extern uint8_t display_flags;
bool process_record_user_luna(uint16_t keycode, keyrecord_t *record);
void print_status_luna(void);
void print_status_bongo(void);

char get_bt_advertisement_status_char(void);
void get_bt_connection_status_str(char *status_str, char *host_name);
void update_bt_connection_status_str(void);

#define CON_STATUS_STR_LEN 6
#define CON_HOSTNAME_LEN 5
extern uint8_t ble_con_status[];
extern uint8_t ble_con_hostname[];

#define RGBLIGHT_MODE_NAME_LEN 6
extern uint8_t rgblight_mode_name[RGBLIGHT_MODE_NAME_LEN];
void update_rgblight_mode_name(void);

#define BMP_USER_FLAG_OLED_ON  1
