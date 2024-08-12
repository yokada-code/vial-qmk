/* SPDX-License-Identifier: GPL-2.0-or-later */

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[1][MATRIX_ROWS][MATRIX_COLS] = {{{
    KC_A, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G, KC_H, KC_I, KC_J, KC_K, KC_L, KC_M, KC_N, KC_O, KC_P, KC_R, KC_S, KC_T
}}};

const uint16_t PROGMEM encoder_map[1][NUM_ENCODERS][NUM_DIRECTIONS] = {{{KC_MS_U, KC_MS_D}}};

enum crkbd_layers {
    _QWERTY,
    _LOWER,
    _RAISE,
    _ADJUST,
};

layer_state_t layer_state_set_user(layer_state_t state) {
    // TRI_LAYER_ENABLE option expects QK_TRI_LAYER_* keycodes.
    // Such keycoards cannot be used for Tap-Hold keys, so
    // using this function to implement tri-layers.
    layer_state_t new;
    new = update_tri_layer_state(state, _RAISE, _LOWER, _ADJUST);
    dprintf("state: %u, new: %u\n", (uint8_t)state, (uint8_t)new);
    return new;
}
