#include QMK_KEYBOARD_H
#include "bmp_pin_def.h"
#include "uart_connection.h"
#include "oled.h"
#include "printf.h"
#include <string.h>

#define INNER_BUF_LEN 128
#define UART_SEND_DATA_MAX 125
#define BUF_LEN 256

#include "microshell/core/microshell.h"
#include "microshell/core/msconf.h"
#include "microshell/util/msopt.h"
#include "microshell/util/mscmd.h"
#include "microshell/util/ntlibc.h"

static uint8_t buf[BUF_LEN];
static uint8_t buf_p = 0;
static uart_cb callback[UART_TYPE_MAX] = {0};
static bool is_connection_established = false;

void uart_set_cb(uint8_t type, uart_cb f) {
    callback[type] = f;
}

typedef enum {
    WAIT_PREAMBLE,
    RECV_LEN,
    RECV_TYPE,
    RECV_DATA,
    RECV_SUM,
} recv_state_t;

static void data_recv_cb(uint8_t d) {
    static recv_state_t state = WAIT_PREAMBLE;
    static uint8_t len;
    static uint8_t checksum = 0;
    static uint8_t type;
    switch (state) {
        case WAIT_PREAMBLE:
            if (d == 'a') {
                state = RECV_LEN;
            }
            break;
        case RECV_LEN:
            len = d;
            buf_p = 0;
            state = RECV_TYPE;
            break;
        case RECV_TYPE:
            type = d;
            checksum = d;
            state = RECV_DATA;
            break;
        case RECV_DATA:
            buf[buf_p++] = d;
            checksum ^= d;
            len--;
            if (len == 0){
                state = RECV_SUM;
            }
            break;
        case RECV_SUM:
            if (d != checksum) {
                dprintf("recv failed, invalid checksum %x, expected: %x\n", d, checksum);
            } else {
                dprintf("recv data, len %d, checksum %x\n", buf_p, checksum);
                if (callback[type] == 0) {
                    dprintf("FATAL: No callback function is registered\n");
                } else {
                    callback[type](buf, buf_p);
                }
            }
            state = WAIT_PREAMBLE;
            break;
    }
}

static char calc_checksum(uint8_t type, uint8_t* data, uint8_t len) {
    uint8_t checksum = type;
    for (uint8_t i=0; i<len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

static void send_char_low(uint8_t* data, uint8_t len) {
    BMPAPI->uart.send(data, len);
}

void uart_send(uint8_t type, uint8_t* data, uint8_t len) {
    static uint8_t flush[INNER_BUF_LEN] = {0};
    uint8_t buf[INNER_BUF_LEN];
    uint8_t checksum;

    if (len > UART_SEND_DATA_MAX) {
        dprintf("Assertion ERROR: uart send data is longer than INNER_BUF_LEN\n");
        return;
    } else if (len == 0) {
        dprintf("Assertion ERROR: uart send with no data\n");
        return;
    }

    checksum = calc_checksum(type, data, len);
    buf[0] = 'a';               //preamble
    buf[1] = len;
    buf[2] = type;
    memcpy(&buf[3], data, len);
    buf[len+3] = checksum;

    send_char_low(buf, len+4);
    dprintf("Send data: len: %d, 0x%x\n", len, checksum);
    send_char_low(flush, INNER_BUF_LEN);
}

static void uart_start_connection_cb(uint8_t* data, uint8_t len) {
    if (strcmp("SYN", (char*)data) == 0) {
        uint8_t msg[] = "ACK";
        uart_send(UART_TYPE_ESTABLISH, msg, sizeof(msg));
        is_connection_established = true;
        dprintf("UART: syn received, starting uart communication.\n");
    } else if (strcmp("ACK", (char*)data) == 0) {
        is_connection_established = true;
        dprintf("UART: ack received, starting uart communication.\n");
    }
}

static void uart_start_disconnect_cb(uint8_t* data, uint8_t len) {
    if (strcmp("DIS", (char*)data) == 0) {
        is_connection_established = false;
        dprintf("UART: dis received, stopping uart communication.\n");
    }
}

void uart_start_connection(void) {
    uint8_t msg[] = "SYN";
    uart_send(UART_TYPE_ESTABLISH, msg, sizeof(msg));
}

bool is_uart_established(void) {
    return is_connection_established;
}

void uart_stop_connection(void) {
    uint8_t msg[] = "DIS";
    uart_send(UART_TYPE_DISCONNECT, msg, sizeof(msg));
    is_connection_established = false;
}

void uart_init(void) {
    bmp_uart_config_t uconf = {
        .tx_pin      = (is_keyboard_master()) ? D2 : B6,
        .rx_pin      = (is_keyboard_master()) ? B6 : D2,
        .baudrate    = Baud1200,
        .rx_callback = data_recv_cb,
        .rx_protocol = 1
    };

    uart_set_cb(UART_TYPE_ESTABLISH, uart_start_connection_cb);
    uart_set_cb(UART_TYPE_DISCONNECT, uart_start_disconnect_cb);
    BMPAPI->uart.init(&uconf);
}

MSCMD_USER_RESULT usrcmd_uart(MSOPT *msopt, MSCMD_USER_OBJECT usrobj) {
    char arg[16];
    if (msopt->argc >= 2) {
        msopt_get_argv(msopt, 1, arg, sizeof(arg));
        if (strcmp(arg, "con") == 0) {
            uart_start_connection();
        } else if (strcmp(arg, "dis") == 0) {
            uart_stop_connection();
        } else if (strcmp(arg, "show") == 0) {
            if (is_uart_established()) {
                printf("UART connected\n");
            } else {
                printf("UART disconnected\n");
            }
        }
    } else {
        printf("uart con|dis|show\n");
    }

    return 0;
}
