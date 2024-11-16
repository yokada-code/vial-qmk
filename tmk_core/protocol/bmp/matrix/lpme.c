/*
 * io_expander.c
 *
 *  Created on: 2018/12/16
 *      Author: sekigon-gonnoc
 */

#include <stdint.h>
#include "lpme.h"
#include "quantum.h"
#include "apidef.h"
#include "i2c.h"
#include "matrix.h"
#include "apidef.h"

static const uint8_t LPME_ADDR_LIST[] = {
    I2C_7BIT_ADDR(0x20), I2C_7BIT_ADDR(0x74), I2C_7BIT_ADDR(0x24)};
#define LPME_ADDR_UNKNOWN 0xFF

static const uint8_t lpme_pin_def[24] = {
    0, // pin number in config starts from 1
    0, 1, 0, 0, 0, 0,
    2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13,
    14, 15, 0, 0, 0
};
#define LPME_REG_INPUT0     0x00
#define LPME_REG_OUTPUT0    0x02
#define LPME_REG_CONFIG0    0x06

static uint8_t lpme_init_packet[6];
static bool reconfig_flag = true;
static bool row_prepare_flag = false;

void lpme_init(lpme_config_t * const config)
{
    uint8_t pin;

    lpme_init_packet[0] = 0xFF; // reg 2: Output port register
    lpme_init_packet[1] = 0xFF;
    lpme_init_packet[2] = 0x00; // reg 4: Polarity inversion register
    lpme_init_packet[3] = 0x00;
    lpme_init_packet[4] = 0xFF; // reg 6: Configuration register
    lpme_init_packet[5] = 0xFF;

    // set row pins as output
    for (int row=0; row<config->row_num; row++)
    {
        pin = lpme_pin_def[config->row_pins[row]];
        if (pin < 8)
        {
            lpme_init_packet[0] &= ~(1<<pin);
            lpme_init_packet[4] &= ~(1<<pin);
        }
        else
        {
            lpme_init_packet[1] &= ~(1 << (pin-8));
            lpme_init_packet[5] &= ~(1 << (pin-8));
        }
    }

    config->addr = LPME_ADDR_UNKNOWN;
}

static matrix_row_t lpme_read_col_on_row(lpme_config_t const * const config)
{
    uint16_t col;

    // read all col pins
    if (i2c_readReg(config->addr, LPME_REG_INPUT0,
        (uint8_t*)&col, sizeof(col), 0))
    {
        return 0;
    }

    matrix_row_t retval = 0;

    for (int col_idx=0; col_idx<config->col_num; col_idx++)
    {
        uint8_t pin = lpme_pin_def[config->col_pins[col_idx]];
        retval |= (col & (1 << pin) ?  0 : (1 << col_idx));
    }

    return retval;
}

static bool lpme_first_scan(lpme_config_t * const config)
{
    if (config->addr == LPME_ADDR_UNKNOWN)
    {
        for (int idx = 0;
             idx < sizeof(LPME_ADDR_LIST) / sizeof(LPME_ADDR_LIST[0]);
             idx++)
        {
            uint8_t i2c_res = i2c_write_register(
                LPME_ADDR_LIST[idx], LPME_REG_OUTPUT0, lpme_init_packet, 1, 0);
            if (i2c_res == 0)
            {
                config->addr = LPME_ADDR_LIST[idx];
                break;
            }
        }
    }

    if (row_prepare_flag == false)
    {
        // set all row pins to low
        i2c_write_register(config->addr, LPME_REG_OUTPUT0,
            lpme_init_packet, 2, 0);
        row_prepare_flag = true;
    }

    if (reconfig_flag)
    {
        // reset all pins I/O setting
        i2c_write_register(config->addr, LPME_REG_CONFIG0,
            &lpme_init_packet[4], 2, 0);
        reconfig_flag = false;
    }

    uint16_t col;

    // read all pin state
    i2c_readReg(config->addr, LPME_REG_INPUT0,
        (uint8_t*)&col, sizeof(col), 0);

    for (int row_idx=0; row_idx<config->row_num; row_idx++)
    {
        uint8_t pin = lpme_pin_def[config->row_pins[row_idx]];
        if ((col & (1 << pin)) != 0)
        {
            // if row pin is high (though it should be set to low),
            // reconfig is required
            reconfig_flag = true;
            break;
        }
    }

    for (int col_idx=0; col_idx<config->col_num; col_idx++)
    {
        uint8_t pin = lpme_pin_def[config->col_pins[col_idx]];
        if ((col & (1 << pin)) == 0)
        {
            // some switch is pressed
            return true;
        }
    }

    return false;
}

static matrix_row_t lpme_read_row(lpme_config_t const * const config, uint8_t row)
{
    uint8_t send_dat[2] = {0xFF, 0xFF};

    uint8_t pin = lpme_pin_def[config->row_pins[row]];
    if (pin < 8)
    {
        send_dat[0] &= ~(1<<pin);
    }
    else
    {
        send_dat[1] &= ~(1<<(pin-8));
    }

    // set a row pin to low
    i2c_write_register(config->addr, LPME_REG_OUTPUT0,
        send_dat, sizeof(send_dat), 0);

    return lpme_read_col_on_row(config);
}

extern const uint8_t MAINTASK_INTERVAL;
uint32_t lpme_scan(lpme_config_t *const config, matrix_row_t *rows,
                   bool wakeup) {
    uint32_t        change                 = 0;
    static uint32_t run_mode               = 0;
    static uint32_t interval_start_time    = 0;
    static uint32_t last_key_press_time    = 0;
    const uint32_t  sleep_enter_threthold1 = 10000;
    const uint32_t  sleep1_interval        = MAINTASK_INTERVAL * 3;
    const uint32_t  sleep_enter_threthold2 = 30000;
    const uint32_t  sleep2_interval        = MAINTASK_INTERVAL * 6;

    if (wakeup) {
        run_mode            = 0;
        last_key_press_time = timer_read32();
        interval_start_time = timer_read32();
        dprint("LPME wake up\n");
    }

    if (run_mode == 0) {
    } else if (run_mode == 1) {
        if (timer_elapsed32(interval_start_time) < sleep1_interval) {
            // skip scan
            return 0;
        } else {
            interval_start_time = timer_read32();
        }
    } else if (run_mode == 2) {
        if (timer_elapsed32(interval_start_time) < sleep2_interval) {
            // skip scan
            return 0;
        } else {
            interval_start_time = timer_read32();
        }
    }

    i2c_init();
    matrix_row_t row;
    if (lpme_first_scan(config)) {
        // some switches are pressed
        for (int row_idx = 0; row_idx < config->row_num; row_idx++) {
            row = lpme_read_row(config, row_idx);
            if (rows[row_idx] ^ row) {
                rows[row_idx] = row;
                change        = 1;
            }
        }
        row_prepare_flag    = false;
        last_key_press_time = timer_read32();
    } else {
        // no switch is pressed
        for (int row_idx = 0; row_idx < config->row_num; row_idx++) {
            if (rows[row_idx]) {
                rows[row_idx] = 0;
                change        = 1;
            }
        }
    }

    if (timer_elapsed32(last_key_press_time) > sleep_enter_threthold2) {
        if (run_mode != 2) {
            dprint("LPME enter sleep mode2\n");
        }
        run_mode = 2;
    } else if (timer_elapsed32(last_key_press_time) > sleep_enter_threthold1) {
        if (run_mode != 1) {
            dprint("LPME enter sleep mode1\n");
        }
        run_mode = 1;
    } else {
        if (run_mode != 0) {
            dprint("LPME wake up\n");
        }
        run_mode = 0;
    }

    i2c_uninit();

    // charge to LPME
    gpio_set_pin_output_push_pull(CONFIG_PIN_SCL);
    gpio_write_pin_high(CONFIG_PIN_SCL);

    return change;
}
