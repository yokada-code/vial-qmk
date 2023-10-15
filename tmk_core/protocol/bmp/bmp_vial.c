// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "print.h"
#include "via.h"
#include "vial.h"
#include "vial_generated_keyboard_definition.h"
#include "matrix.h"

#include "bmp.h"
#include "bmp_vial.h"
#include "bmp_flash.h"
#include "raw_hid.h"
#include "apidef.h"

#define FLASH_PAGE_ID_VIAL 2

_Static_assert(sizeof(flash_vial_data_t) >= BMP_USER_FLASH_PAGE_SIZE, "Invalid size");
flash_vial_data_t flash_vial_data;

void bmp_vial_data_init(void) {
    const uint8_t lzma_header[] = {0xfd, 0x37, 0x71, 0x58, 0x5a};
    BMPAPI->flash.read(FLASH_PAGE_ID_VIAL * BMP_USER_FLASH_PAGE_SIZE, (void *)&flash_vial_data, BMP_USER_FLASH_PAGE_SIZE);
    if (flash_vial_data.len > sizeof(flash_vial_data.data) || (memcmp(flash_vial_data.data, lzma_header, sizeof(lzma_header)) != 0)) {
        // Load default data
        flash_vial_data.len = sizeof(keyboard_definition);
        memcpy(flash_vial_data.data, keyboard_definition, sizeof(keyboard_definition));

        // Write to flash
        flash_erase_page(FLASH_PAGE_ID_VIAL);
        flash_write_page(FLASH_PAGE_ID_VIAL, (uint32_t *)&flash_vial_data);
    }
}

const bmp_api_config_t *bmp_vial_get_config(void) {
    return &flash_vial_data.bmp_config;
}

void bmp_vial_set_config(void) {
    // Write to flash
    flash_erase_page(FLASH_PAGE_ID_VIAL);
    flash_write_page(FLASH_PAGE_ID_VIAL, (uint32_t *)&flash_vial_data);
}

static bool pre_raw_hid_receive(uint8_t *msg, uint8_t len) {
    bool _continue = true;
    if (msg[0] == id_get_keyboard_value && msg[1] == id_switch_matrix_state) {
        _continue = false;
        uint8_t i = 2;
        for (uint8_t row = 0; row < bmp_config->matrix.rows; row++) {
            matrix_row_t value = matrix_get_row(row);

            if (bmp_config->matrix.cols > 24) {
                msg[i++] = (value >> 24) & 0xFF;
                if (i >= 32) break;
            }

            if (bmp_config->matrix.cols > 16) {
                msg[i++] = (value >> 16) & 0xFF;
                if (i >= 32) break;
            }

            if (bmp_config->matrix.cols > 8) {
                msg[i++] = (value >> 8) & 0xFF;
                if (i >= 32) break;
            }

            msg[i++] = value & 0xFF;
            if (i >= 32) break;
        }
    }

    // Override vial protocol
    if (msg[0] != 0xfe) {
        switch (msg[1]) {
            case vial_get_size: {
                _continue   = false;
                uint32_t sz = flash_vial_data.len;
                msg[0]      = sz & 0xFF;
                msg[1]      = (sz >> 8) & 0xFF;
                msg[2]      = (sz >> 16) & 0xFF;
                msg[3]      = (sz >> 24) & 0xFF;
                break;
            }

            case vial_get_def: {
                _continue      = false;
                uint32_t page  = msg[2] + (msg[3] << 8);
                uint32_t start = page * VIAL_RAW_EPSIZE;
                uint32_t end   = start + VIAL_RAW_EPSIZE;
                if (end < start || start >= flash_vial_data.len) break;
                if (end > flash_vial_data.len) end = flash_vial_data.len;
                memcpy(msg, &flash_vial_data.data[start], end - start);
                break;
            }
        }
    }

    if (!_continue) {
        raw_hid_send(msg, len);
    }

    return _continue;
}

void bmp_raw_hid_receive(const uint8_t *data, uint8_t len) {
    static uint8_t via_data[32];
    if (len > sizeof(via_data) + 1) {
        printf("<raw_hid>Too large packet");
        return;
    }

    memcpy(via_data, data, len - 1);

    if (pre_raw_hid_receive(via_data, len - 1)) {
        raw_hid_receive(via_data, len - 1);
    }
}
