// Copyright 2023 sekigon-gonnoc
/* SPDX-License-Identifier: GPL-2.0-or-later */

// BMP
#include "state_controller.h"
#include "bmp_host_driver.h"

int           sleep_enter_counter     = -1;
int           reset_counter           = -1;
int           bootloader_jump_counter = -1;

static bool is_usb_connected_ = false;
static bool is_usb_powered_   = false;
static bool is_ble_connected_ = false;

bool is_usb_connected(void) {
    return is_usb_connected_;
}
bool is_usb_powered(void) {
    return is_usb_powered_;
}
bool is_ble_connected(void) {
    return is_ble_connected_;
}

__attribute__((weak)) void bmp_before_sleep(void) {}
__attribute__((weak)) void bmp_enter_sleep(void) {
    bmp_before_sleep();
    BMPAPI->app.enter_sleep_mode();
}

__attribute__((weak)) void bmp_state_change_cb_user(bmp_api_event_t event) {}
__attribute__((weak)) void bmp_state_change_cb_kb(bmp_api_event_t event) {
    bmp_state_change_cb_user(event);
}

bmp_error_t bmp_state_change_cb(bmp_api_event_t event)
{
  switch (event)
  {
    case USB_CONNECTED:
        is_usb_powered_ = true;
        break;

    case USB_HID_READY:
        is_usb_powered_   = true;
        is_usb_connected_ = true;
        if (!is_ble_connected()) {
            select_usb();
        }
        // update_config_files();
        break;

    case USB_DISCONNECTED:
        is_usb_powered_   = false;
        is_usb_connected_ = false;
        if (is_ble_connected()) {
            select_ble();
        }
        break;

    case BLE_ADVERTISING_START:
    //   bmp_indicator_set(INDICATOR_ADVERTISING, 0);
      break;

    case BLE_ADVERTISING_STOP:
    //   bmp_indicator_set(INDICATOR_TURN_OFF, 0);

      if (!is_usb_powered()) {
          sleep_enter_counter = 1;
      }
      break;

    case BLE_CONNECTED:
        is_ble_connected_ = true;
        if (!is_usb_connected()) {
            select_ble();
        }
        // bmp_indicator_set(INDICATOR_CONNECTED, 0);
        break;

    case BLE_DISCONNECTED:
        is_ble_connected_ = false;
        // Disable below code because BLE could be disconnected unintentionally
        // if (is_usb_connected()) {
        //     select_usb();
        // }
        break;

    default:
      break;
  }

  bmp_state_change_cb_kb(event);

  return BMP_OK;
}

void bmp_mode_transition_check(void) {
    // sleep flag check
    if (sleep_enter_counter > 0)
    {
        sleep_enter_counter--;
        if (sleep_enter_counter == 0)
        {
            bmp_enter_sleep();
        }
    }

    // reset flag check
    if (reset_counter > 0)
    {
        reset_counter--;
        if (reset_counter == 0)
        {
            // reset without safemode flag
            BMPAPI->app.reset(0);
        }
    }

    // bootloader jump flag check
    if (bootloader_jump_counter > 0) {
        bootloader_jump_counter--;
        if (bootloader_jump_counter == 0) {
            BMPAPI->bootloader_jump();
        }
    }
}
