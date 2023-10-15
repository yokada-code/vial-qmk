
#pragma once
#include <stdint.h>
#include "apidef.h"
#include "action.h"
#include "quantum_keycodes.h"

bool bmp_config_overwrite(bmp_api_config_t const *const config_on_storage,
                          bmp_api_config_t *const       keyboard_config);
void bmp_mode_transition_check(void);
void bmp_keyboard_task(void);
void bmp_init(void);
int  bmp_dynamic_keymap_init(void);

extern const bmp_api_config_t default_config;
extern const bmp_api_config_t *bmp_config;

const char *bmp_get_version_info(void);
bool        is_safe_mode(void);

void bmp_via_receive_cb(uint8_t *data, uint8_t length,
                        int (*raw_hid_send)(const uint8_t *data,
                                            uint8_t        length));

bool is_safe_mode(void);
bool process_record_bmp(uint16_t keycode, keyrecord_t *record);

extern int sleep_enter_counter;
extern int reset_counter;
void bmp_enter_sleep(void);
void bmp_before_sleep(void);
