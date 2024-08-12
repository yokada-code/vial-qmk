#pragma once

#include "bmp_custom_keycodes.h"

enum custom_keycodes {
    UARTINI = BMP_SAFE_RANGE,
    UARTDIS
};

enum crkbd_layers {
    _QWERTY,
    _LOWER,
    _RAISE,
    _ADJUST,
};
