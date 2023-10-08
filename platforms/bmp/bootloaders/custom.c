// Copyright 2023 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bootloader.h"

// TODO: bmp

__attribute__((weak)) void bootloader_jump(void) {}
__attribute__((weak)) void mcu_reset(void) {}

__attribute__((weak)) void enter_bootloader_mode_if_requested(void) {}
