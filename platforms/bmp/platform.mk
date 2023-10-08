
# ARM toolchain specific configuration
TOOLCHAIN ?= arm-none-eabi-

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