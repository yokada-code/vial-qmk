// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "timer.h"

// TODO: bmp

static uint32_t timer_cnt;

void timer_tick(uint32_t tick) {
    timer_cnt += tick;
}

void timer_init(void) {
    timer_cnt = 0;
}
void timer_clear(void) {
    timer_cnt = 0;
}

uint16_t timer_read(void) {
    return timer_cnt & 0xffff;
}

uint32_t timer_read32(void) {
    return timer_cnt;
}

uint16_t timer_elapsed(uint16_t last) {
    return TIMER_DIFF_16(timer_read(), last);
}

uint32_t timer_elapsed32(uint32_t last) {
    return TIMER_DIFF_32(timer_read32(), last);
}