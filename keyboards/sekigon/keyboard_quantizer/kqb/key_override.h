
#pragma once

typedef enum {
    NO_KEY_OS_OVERRIDE,
    JP_KEY_ON_US_OS_OVERRIDE,
    US_KEY_ON_JP_OS_OVERRIDE,
} JP_US_OVERRIDE;

void set_key_override(JP_US_OVERRIDE);