
#pragma once

#define BMP_SETTINGS_SIZE 64

typedef enum {
    BMP_KEY_OS_OVERRIDE_DISABLE,
    BMP_US_KEY_JP_OS_OVERRIDE,
    BMP_JP_KEY_US_OS_OVERRIDE,
} key_os_override_t;

typedef struct {
    key_os_override_t key_os_override;
} bmp_settings_t;

void              bmp_settings_init(void);
void              bmp_set_key_os_override(key_os_override_t override);
key_os_override_t bmp_get_key_os_override(void);