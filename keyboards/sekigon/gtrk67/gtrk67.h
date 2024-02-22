#pragma once

#include "quantum.h"

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t scroll;
    uint8_t surface;
    uint8_t motion_flag;
} trackball_info_t;

typedef enum {
    TRACKBALL_NONE,
    TRACKBALL_ADNS,
    TRACKBALL_BTO,
} trackball_type_t;

const trackball_info_t* get_trackball_info(void);
extern trackball_type_t trackball_init_flag;
void                    override_mouse_report(report_mouse_t report);
