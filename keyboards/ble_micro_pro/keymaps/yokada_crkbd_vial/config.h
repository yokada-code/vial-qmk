/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

#define VIAL_KEYBOARD_UID {0x05, 0xE4, 0xA1, 0x7F, 0xDC, 0x87, 0xCB, 0x2A}
#define VIAL_UNLOCK_COMBO_ROWS { 0, 1 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 1 }

#define WPM_UNFILTERED
#define OLED_TIMEOUT 60000

#ifndef __ASSEMBLER__
#    include "microshell/core/msconf.h"
#    include "microshell/util/mscmd.h"
#    define USER_DEFINED_MSCMD  {"uart", usrcmd_uart, "UART connection"},
MSCMD_USER_RESULT usrcmd_uart(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
#endif
