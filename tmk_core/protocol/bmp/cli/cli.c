#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "microshell/core/microshell.h"
#include "microshell/core/msconf.h"
#include "microshell/util/msopt.h"
#include "microshell/util/mscmd.h"
#include "microshell/util/ntlibc.h"

// QMK headers
#include "printf.h"
#include "debug.h"

// BMP headers
#include "cli.h"
#include "apidef.h"
#include "bmp.h"

void              cli_puts(const char *str);
static MICROSHELL microshell;
static MSCMD      mscmd;
static cli_app_t  cli_app = {NULL, NULL};

static MSCMD_USER_RESULT usrcmd_help(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
static MSCMD_USER_RESULT usrcmd_bootloader(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);
static MSCMD_USER_RESULT usrcmd_debug_enable(MSOPT *msopt, MSCMD_USER_OBJECT usrobj);

static const MSCMD_COMMAND_TABLE table[] = {{"help", usrcmd_help, "Show this message"},
                                            {"dfu", usrcmd_bootloader, "Jump to bootloader"},
                                            {"debug", usrcmd_debug_enable, "Debug print setting"},
#ifdef USER_DEFINED_MSCMD
                                            USER_DEFINED_MSCMD
#endif
};

void cli_puts(const char *str) {
    BMPAPI->usb.serial_puts((const uint8_t *)str, strlen(str));
}

void cli_init(void) {
    microshell_init(&microshell, BMPAPI->usb.serial_putc, BMPAPI->usb.serial_byte_to_read, BMPAPI->usb.serial_getc, NULL);
    mscmd_init(&mscmd, table, sizeof(table) / sizeof(table[0]), NULL);
}

void cli_exec(void) {
    static char  buf[MSCONF_MAX_INPUT_LENGTH];
    static char *cur       = buf;
    static bool  show_info = false;

    if (cli_app.func != NULL) {
        cli_app.func(cli_app.data);
        return;
    }

    if (!show_info) {
        if (is_safe_mode()) {
            printf("bmp@safemode>");
        } else {
            printf("bmp@%s>", BMPAPI->app.get_config()->device_info.name);
        }
        show_info = true;
    }

    cur = microshell_getline(&microshell, buf, cur, MSCONF_MAX_INPUT_LENGTH - (cur - buf));
    if (cur == buf + MSCONF_MAX_INPUT_LENGTH) {
        MSCMD_USER_RESULT r;
        mscmd_execute(&mscmd, buf, &r);
        memset(buf, 0, MSCONF_MAX_INPUT_LENGTH);
        cur       = buf;
        show_info = false;
    }
}

static MSCMD_USER_RESULT usrcmd_help(MSOPT *msopt, MSCMD_USER_OBJECT usrobj) {
    for (int i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
        cli_puts(table[i].argv0);
        microshell.uart_putc('\t');
        cli_puts(table[i].desc);
        cli_puts("\r\n");
    }
    return 0;
}

static MSCMD_USER_RESULT usrcmd_bootloader(MSOPT *msopt, MSCMD_USER_OBJECT usrobj) {
    BMPAPI->bootloader_jump();
    return 0;
}

static MSCMD_USER_RESULT usrcmd_debug_enable(MSOPT *msopt, MSCMD_USER_OBJECT usrobj) {
    char arg[16];
    if (msopt->argc >= 2) {
        msopt_get_argv(msopt, 1, arg, sizeof(arg));
        if (strcmp(arg, "on") == 0) {
            debug_enable = true;
        } else if (strcmp(arg, "keyboard") == 0) {
            debug_enable   = true;
            debug_keyboard = true;
        } else if (strcmp(arg, "mouse") == 0) {
            debug_enable = true;
            debug_mouse  = true;
        } else if (strcmp(arg, "all") == 0) {
            debug_enable   = true;
            debug_keyboard = true;
            debug_mouse    = true;
        } else if (strcmp(arg, "off") == 0) {
            debug_enable   = false;
            debug_keyboard = false;
            debug_mouse    = false;
        }
    } else {
        printf("Set debug option [on|off]\r\n");
    }
    return 0;
}