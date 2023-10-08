#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "microshell/core/microshell.h"
#include "microshell/core/msconf.h"
#include "microshell/util/msopt.h"
#include "microshell/util/mscmd.h"
#include "microshell/util/ntlibc.h"

#include "printf.h"

#include "cli.h"
#include "apidef.h"
#include "bmp.h"

void              cli_puts(const char *str);
static MICROSHELL microshell;
static MSCMD      mscmd;
static cli_app_t cli_app = {NULL, NULL};

static const MSCMD_COMMAND_TABLE table[] = {
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
