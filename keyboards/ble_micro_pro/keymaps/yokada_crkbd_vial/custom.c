#include QMK_KEYBOARD_H

#include "custom.h"
#include "bmp_indicator_led.h"

uint8_t get_advertise_to(void) {
    return 255;  //not implemented
}

// Dummy function always returning 0 to enable oled refresh
int bmp_indicator_user_pattern(uint32_t time_ms, int32_t option) { return 0; }


void keyboard_post_init_user(void) {
    bmp_indicator_set(INDICATOR_USER, 0);
}
