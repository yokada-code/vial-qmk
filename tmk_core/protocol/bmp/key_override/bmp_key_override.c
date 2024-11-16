// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "bmp_key_override.h"

#include "vial.h"

#define BMP_KEY_OVERRIDE_ENTRIES 25

extern const key_override_t **key_overrides;
static key_override_t         bmp_override[BMP_KEY_OVERRIDE_ENTRIES]                                  = {0};
static uint8_t                bmp_override_cnt                                                        = 0;
static const key_override_t  *override_ptrs[VIAL_KEY_OVERRIDE_ENTRIES + BMP_KEY_OVERRIDE_ENTRIES + 1] = {0};
void bmp_key_override_init(void) {
    // copy vial override
    for (size_t i = 0; i < VIAL_KEY_OVERRIDE_ENTRIES; ++i) {
        override_ptrs[i] = key_overrides[i];
    }
    
    // regisiter bmp override
    for (size_t i = 0; i < BMP_KEY_OVERRIDE_ENTRIES; ++i) {
        override_ptrs[i + VIAL_KEY_OVERRIDE_ENTRIES] = &bmp_override[i];
    }
    
    // swap reference
    key_overrides = override_ptrs;
}


int register_bmp_key_override(const key_override_t *override) {
    if (bmp_override_cnt >= BMP_KEY_OVERRIDE_ENTRIES) {
        return -1;
    }

    override_ptrs[VIAL_KEY_OVERRIDE_ENTRIES + bmp_override_cnt++]   = override;
    override_ptrs[VIAL_KEY_OVERRIDE_ENTRIES + bmp_override_cnt + 1] = override;
    return 0;
}

void remove_all_bmp_key_overrides(void) {
    for (int i = 0; i < BMP_KEY_OVERRIDE_ENTRIES; i++) {
        override_ptrs[VIAL_KEY_OVERRIDE_ENTRIES + i] = NULL;
    }
    bmp_override_cnt = 0;
}
