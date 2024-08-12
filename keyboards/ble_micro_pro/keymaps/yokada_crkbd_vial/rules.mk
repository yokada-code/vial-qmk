VIA_ENABLE = yes
VIAL_ENABLE = yes
VIAL_INSECURE = yes

OLED_ENABLE = yes
OLED_DRIVER = ssd1306

WPM_ENABLE = yes

SRC += custom.c oled_bongo.c oled_luna.c oled.c matrix_uart.c uart_connection.c
