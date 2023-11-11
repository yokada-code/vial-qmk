
#include "eeprom.h"

#include "bmp_settings.h"
#include "bmp_key_override.h"

#define KEY_OS_OVERRIDE_ADDR (TOTAL_EEPROM_BYTE_COUNT - BMP_SETTINGS_SIZE + offsetof(bmp_settings_t, key_os_override))

void bmp_settings_init(void) {
    bmp_set_key_os_override(bmp_get_key_os_override());
}

void bmp_set_key_os_override(key_os_override_t override) {
    switch (override) {
        case BMP_KEY_OS_OVERRIDE_DISABLE:
            remove_all_bmp_key_overrides();
            eeprom_update_byte((uint8_t *)KEY_OS_OVERRIDE_ADDR, BMP_KEY_OS_OVERRIDE_DISABLE);
            break;
        case BMP_US_KEY_JP_OS_OVERRIDE:
            register_us_key_on_jp_os_overrides();
            eeprom_update_byte((uint8_t *)KEY_OS_OVERRIDE_ADDR, BMP_US_KEY_JP_OS_OVERRIDE);
            break;
        case BMP_JP_KEY_US_OS_OVERRIDE:
            register_jp_key_on_us_os_overrides();
            eeprom_update_byte((uint8_t *)KEY_OS_OVERRIDE_ADDR, BMP_JP_KEY_US_OS_OVERRIDE);
            break;
    }
}

key_os_override_t bmp_get_key_os_override(void) {
    return eeprom_read_byte((uint8_t *)KEY_OS_OVERRIDE_ADDR);
}