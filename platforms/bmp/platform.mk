SRC += $(PLATFORM_COMMON_DIR)/wait.c
SRC += $(PLATFORM_COMMON_DIR)/drivers/eeprom/eeprom_bmp.c

CONFIG_H += $(PLATFORM_COMMON_DIR)/config.h

MCUFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant
# Linker flags
LDFLAGS += -mthumb -mabi=aapcs

# LDSCRIPT_PATH := $(shell dirname "$(LDSCRIPT)")
# LDFLAGS += -L$(LDSCRIPT_PATH) -T$(LDSCRIPT)

LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys
LDFLAGS += -Xlinker --print-memory-usage

# ARM toolchain specific configuration
TOOLCHAIN ?= arm-none-eabi-

# Assembler flags
ASFLAGS  += $(SHARED_ASFLAGS) $(TOOLCHAIN_ASFLAGS)

# C Compiler flags
CFLAGS   += $(SHARED_CFLAGS) $(TOOLCHAIN_CFLAGS)

# C++ Compiler flags
CXXFLAGS += $(CFLAGS) $(SHARED_CXXFLAGS) $(TOOLCHAIN_CXXFLAGS) -fno-rtti

# Linker flags
LDFLAGS  += $(SHARED_LDFLAGS) $(SHARED_LDSYMBOLS) $(TOOLCHAIN_LDFLAGS) $(TOOLCHAIN_LDSYMBOLS) $(MCUFLAGS)

# Tell QMK that we are hosting it on BLE Micro Pro.
OPT_DEFS += -DPROTOCOL_BMP

# Construct GCC toolchain
CC      = $(CC_PREFIX) $(TOOLCHAIN)gcc
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump
SIZE    = $(TOOLCHAIN)size
AR      = $(TOOLCHAIN)ar
NM      = $(TOOLCHAIN)nm
HEX     = $(OBJCOPY) -O $(FORMAT)
EEP     =
BIN     = $(OBJCOPY) -O binary