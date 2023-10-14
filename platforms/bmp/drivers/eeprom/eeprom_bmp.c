// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "eeprom_driver.h"
#include "wait.h"
#include "print.h"

#include "bmp_flash.h"
#include "apidef.h"

#define BMP_FLASH_DRIVER_MAGIC 0xBEC52840

enum {
    FLASH_PAGE_ID_CONTROL,
    FLASH_PAGE_ID_DATA,
    FLASH_PAGE_ID_VIAL,
};
typedef struct {
    uint32_t magic;
    struct {
        uint16_t addr;
        uint16_t data;
    } log[(BMP_USER_FLASH_PAGE_SIZE - 4) / 4];

} flash_page_control_t;
_Static_assert(sizeof(flash_page_control_t) == BMP_USER_FLASH_PAGE_SIZE, "Invalid size");

typedef struct {
    uint8_t data[BMP_USER_FLASH_PAGE_SIZE];
} flash_page_data_t;
_Static_assert(sizeof(flash_page_data_t) == BMP_USER_FLASH_PAGE_SIZE, "Invalid size");

static flash_page_data_t    flash_data;
static flash_page_control_t flash_control;
static uint32_t             log_write_idx;
static const uint32_t       control_log_max = sizeof(flash_control.log) / sizeof(flash_control.log[0]);

static void truncate_flash_pages(void) {
    printf("Truncate flash pages\n");
    // Erase data
    flash_erase_page(FLASH_PAGE_ID_DATA);
    // Write data
    flash_write_page(FLASH_PAGE_ID_DATA, (uint32_t *)flash_data.data);
    // Erase log
    flash_erase_page(FLASH_PAGE_ID_CONTROL);
    wait_ms(10);
    memset(&flash_control, 0xff, sizeof(flash_control));
    log_write_idx = 0;
    // Write magic number
    flash_control.magic = BMP_FLASH_DRIVER_MAGIC;
    flash_write_dword(FLASH_PAGE_ID_CONTROL * BMP_USER_FLASH_PAGE_SIZE, &flash_control.magic);
}

void eeprom_driver_init(void) {
    flash_read(FLASH_PAGE_ID_CONTROL * BMP_USER_FLASH_PAGE_SIZE, (uint8_t *)&flash_control, BMP_USER_FLASH_PAGE_SIZE);
    log_write_idx = 0;

    if (flash_control.magic == BMP_FLASH_DRIVER_MAGIC) {
        flash_read(FLASH_PAGE_ID_DATA * BMP_USER_FLASH_PAGE_SIZE, (uint8_t *)&flash_data, BMP_USER_FLASH_PAGE_SIZE);

        // Apply write log
        for (int ofs = 0; ofs < control_log_max; ofs++) {
            uint16_t addr = flash_control.log[ofs].addr;
            uint16_t data = flash_control.log[ofs].data;
            if (addr > BMP_USER_FLASH_PAGE_SIZE - 2) {
                break;
            } else {
                *(uint16_t *)(&flash_data.data[addr]) = data;
            }
            log_write_idx++;
        }
        // printf("read %ld log\n", log_write_idx);
    } else {
        // printf("magic %lx\n", flash_control.magic);
        truncate_flash_pages();
    }
}

void eeprom_driver_erase(void) {
    truncate_flash_pages();
    // flash_erase_page(FLASH_PAGE_ID_CONTROL);
    // flash_erase_page(FLASH_PAGE_ID_DATA);
}

void eeprom_read_block(void *buf, const void *addr, size_t len) {
    memcpy(buf, &flash_data.data[(uint32_t)addr], len);
}

static void bmp_flash_write_word(uint16_t data, uint32_t addr) {
    if (addr > BMP_USER_FLASH_PAGE_SIZE - 2) {
        return;
    }

    if (log_write_idx >= control_log_max) {
        truncate_flash_pages();
    }

    if (*(uint16_t *)&flash_data.data[addr] == data) {
        // data is already written
        return;
    }

    *(uint16_t *)&flash_data.data[addr] = data;

    if (flash_control.log[log_write_idx].addr != 0xffff || flash_control.log[log_write_idx].data != 0xffff) {
        // printf("%04x, %04x\n", flash_control.log[log_write_idx].addr, flash_control.log[log_write_idx].data);
        truncate_flash_pages();
    }

    flash_control.log[log_write_idx].addr = addr;
    flash_control.log[log_write_idx].data = data;
    // Write log
    flash_write_dword(offsetof(flash_page_control_t, log) + log_write_idx * sizeof(uint32_t), (uint32_t *)&flash_control.log[log_write_idx]);
    log_write_idx++;
}

static void bmp_flash_write_byte(uint8_t data, uint32_t addr) {
    uint32_t aligned_addr = addr & 0xfffffffe;
    uint16_t wdata        = 0;

    if (addr & 1) {
        wdata = (flash_data.data[aligned_addr]) | ((uint16_t)data << 8);
    } else {
        wdata = ((uint16_t)flash_data.data[aligned_addr + 1] << 8) | ((uint16_t)data);
    }
    bmp_flash_write_word(wdata, aligned_addr);
}

void eeprom_write_block(const void *buf, void *_addr, size_t len) {
    uint32_t       addr   = (uint32_t)_addr;
    const uint8_t *u8buff = (uint8_t *)buf;
    uint32_t       widx   = 0;
    if ((addr & 1) == 1) {
        bmp_flash_write_byte(u8buff[widx], addr);
        addr++;
        widx++;
        len--;
    }

    while (len > 0) {
        if (len >= 2) {
            uint16_t wd = u8buff[widx] | ((uint16_t)u8buff[widx + 1] << 8);
            bmp_flash_write_word(wd, addr);
            addr += 2;
            widx += 2;
            len -= 2;
        } else if (len == 1) {
            bmp_flash_write_byte(u8buff[widx], addr);
            addr++;
            widx++;
            len--;
        }
    }
}
