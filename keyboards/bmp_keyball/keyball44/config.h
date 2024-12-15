/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// BMP specific configuration
#define MATRIX_ROWS_DEFAULT (4 * 2)
#define MATRIX_COLS_DEFAULT 6
#define THIS_DEVICE_ROWS (4 * 2)
#define THIS_DEVICE_COLS 6

#ifdef OLED_ENABLE
#    define SPLIT_OLED_ENABLE
#endif

#ifndef OLED_FONT_H
#    define OLED_FONT_H "keyboards/bmp_keyball/lib/logofont/logofont.c"
#    define OLED_FONT_START 32
#    define OLED_FONT_END 195
#endif
