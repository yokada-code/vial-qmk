
#include "ws2812.h"
#include "apidef.h"

void ws2812_init(void) {}

// Setleds for standard RGB
void ws2812_setleds(LED_TYPE *ledarray, uint16_t leds) {
    BMPAPI->ws2812.setleds_pin((bmp_api_led_t *)ledarray, leds, BMPAPI->app.get_config()->led.pin);
}
