#include QMK_KEYBOARD_H
#include "pointing_device.h"
#include "pointing_device_auto_mouse.h"
#include "bmp.h"
#include "bmp_custom_keycodes.h"

uint8_t tb_scrl_flag = false;
int16_t tb_div = 1;

enum {
    SCROLL_MODE = BMP_SAFE_RANGE,
};

const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {KC_NO};

void pointing_device_init_user(void) {
    set_auto_mouse_layer(3);
    set_auto_mouse_enable(true);
}

bool is_mouse_record_user(uint16_t keycode, keyrecord_t* record) {
    switch(keycode) {
        case KC_LEFT_CTRL...KC_RIGHT_GUI:
            return true;
        case SCROLL_MODE:
            return true;  
        default:
            return false;
    }
    return false;
}

static inline int sgn(int16_t x) {
    if (x > 0) {
        return 1;
    } else if (x < 0) {
        return -1;
    } else {
        return 0;
    }
}

void matrix_scan_user(void) {
    static int16_t          x_surplus, y_surplus;
    static uint32_t         cnt;
    const trackball_info_t *tb_info = get_trackball_info();
    report_mouse_t          mouse_rep;

    if (tb_info->motion_flag & 0x80) {
        if (!tb_scrl_flag) {
            mouse_rep.h = 0;
            mouse_rep.v = 0;
            mouse_rep.x = (tb_info->x + x_surplus) / tb_div;
            mouse_rep.y = (tb_info->y + y_surplus) / tb_div;

            x_surplus = (tb_info->x + x_surplus) % tb_div;
            y_surplus = (tb_info->y + y_surplus) % tb_div;
        } else {
            mouse_rep.h = 1 * sgn((tb_info->x + x_surplus) / tb_div);
            mouse_rep.v = -1 * sgn((tb_info->y + y_surplus) / tb_div);
            mouse_rep.x = 0;
            mouse_rep.y = 0;

            x_surplus = (tb_info->x + x_surplus) % tb_div;
            y_surplus = (tb_info->y + y_surplus) % tb_div;
        }

        if (++cnt % 10 == 0) {
            if (debug_mouse) dprintf("0x%2x: %6d %6d %3d\n", tb_info->motion_flag, tb_info->x, tb_info->y, tb_info->surface);
        }

        override_mouse_report(mouse_rep);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case SCROLL_MODE:
                dprintf("enable scroll mode\n");
                tb_scrl_flag = true;
                return false;
        }
    } else {
        switch (keycode) {
            case SCROLL_MODE:
                dprintf("disable scroll mode\n");
                tb_scrl_flag = false;
                return false;
        }
    }

    return true;
}