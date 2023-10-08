// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <string.h>

#include "eeprom_driver.h"

// TODO: bmp

void eeprom_driver_init(void) {
    /* Any initialisation code */
 }

void eeprom_driver_erase(void) {
    /* Wipe out the EEPROM, setting values to zero */
}

void eeprom_read_block(void *buf, const void *addr, size_t len) {
    /*
        Read a block of data:
            buf: target buffer
            addr: 0-based offset within the EEPROM
            len: length to read
     */
}

void eeprom_write_block(const void *buf, void *addr, size_t len) {
    /*
        Write a block of data:
            buf: target buffer
            addr: 0-based offset within the EEPROM
            len: length to write
     */
}
