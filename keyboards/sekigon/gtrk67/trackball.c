// Copyright 2024 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "trackball.h"

#include "gpio.h"
#include "spi.h"
#include "debug.h"


static uint8_t init_data[] = {
    0x0A, 0xB8, 0x37, 0xF8, 0x38, 0xC7, 0x06, 0x70, 0x07, 0x88, 0x09,
    0x64, 0x0B, 0x09, 0x0C, 0x80, 0x0D, 0x08, 0x0E, 0x8B, 0x0F, 0x16,
    0x10, 0x0A, 0x11, 0x1F, 0x12, 0x0E, 0x14, 0x42, 0x15, 0x06, 0x17,
    0x3C, 0x18, 0x0A, 0x19, 0x23, 0x1A, 0x0F, 0x1C, 0x22, 0x1D, 0x22,
    0x1E, 0x09, 0x1F, 0x00, 0x20, 0x46, 0x21, 0x04, 0x22, 0x7D, 0x23,
    0x5A, 0x24, 0x46, 0x27, 0x28, 0x28, 0x3F, 0x29, 0x20, 0x2B, 0x00,
    0x2C, 0x0F, 0x2D, 0x0F, 0x2E, 0x04, 0x2F, 0x47, 0x30, 0x06, 0x31,
    0x2D, 0x32, 0x2D, 0x33, 0x80, 0x34, 0x7F, 0x35, 0x80, 0x36, 0x7F,
    0x37, 0xC0, 0x38, 0x3F, 0x39, 0x08, 0x3B, 0x0A, 0x3C, 0x0D, 0x71,
    0x40, 0x73, 0x32, 0x74, 0x09, 0x75, 0x10, 0x76, 0x08, 0x02, 0x81};

static uint8_t enable_data[] = {0x08, 0x70, 0x02, 0x81};

static uint8_t get_cmd1[] = {0xB3, 0xB4, 0xB5, 0xB6, 0x80};
static uint8_t get_cmd2[] = {0x84, 0x85, 0x80};

static uint8_t sleep_cmd[] = {0x80, 0x08, 0x70, 0x02, 0x89};
static uint8_t wake_cmd[] = {0x08, 0x70, 0x02, 0x81};

int trackball_init(uint8_t tb_cs, uint8_t init1, uint8_t init2) {
    uint8_t recv_data[sizeof(init_data)] = {0};

    init_data[81] = init1;
    init_data[83] = 0xff - init_data[81];
    init_data[85] = init2;
    init_data[87] = 0xff - init_data[85];

    spim_start((uint8_t *)init_data, sizeof(init_data), recv_data,
               sizeof(recv_data), tb_cs);

    return (recv_data[0] & 1) ? 0 : 1;
}

int trackball_enable(uint8_t tb_cs) {
    uint8_t recv_data[sizeof(enable_data)] = {0};

    spim_start((uint8_t *)enable_data, sizeof(enable_data), recv_data,
               sizeof(recv_data), tb_cs);

    return (recv_data[0] == 0x49 && recv_data[1] == 0x70) ? 0 : 1;
}

trackball_data_t trackball_get(uint8_t tb_cs) {
    uint8_t recv_data[sizeof(get_cmd1)]={0};
    trackball_data_t trackball_data;

    spim_start((uint8_t *)get_cmd1, sizeof(get_cmd1), recv_data,
               sizeof(get_cmd1), tb_cs);
    spim_start((uint8_t *)get_cmd2, sizeof(get_cmd2),
               (uint8_t *)&trackball_data, sizeof(get_cmd2), tb_cs);

    return trackball_data;
}

uint8_t trackball_get_status(uint8_t tb_cs) {
    uint8_t recv_data[sizeof(get_cmd1)]={0};

    spim_start((uint8_t *)get_cmd1, sizeof(get_cmd1), recv_data,
               sizeof(get_cmd1), tb_cs);

    return recv_data[0];
}

int trackball_sleep(uint8_t tb_cs) {
    uint8_t recv_data[sizeof(sleep_cmd)] = {0};

    spim_start((uint8_t *)sleep_cmd, sizeof(sleep_cmd), recv_data,
               sizeof(sleep_cmd), tb_cs);

    for (int idx = 0; idx < sizeof(recv_data); idx++) {
        dprintf("%02x ", recv_data[idx]);
    }
    dprintf("\n");

    return 0;
}

int trackball_wakeup(uint8_t tb_cs) {
    uint8_t recv_data[sizeof(wake_cmd)] = {0};

    spim_start((uint8_t *)wake_cmd, sizeof(wake_cmd), recv_data,
               sizeof(wake_cmd), tb_cs);

    for (int idx = 0; idx < sizeof(recv_data); idx++) {
        dprintf("%02x ", recv_data[idx]);
    }
    dprintf("\n");

    return 0;
}
