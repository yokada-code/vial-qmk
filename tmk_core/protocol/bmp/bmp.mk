PROTOCOL_DIR = protocol
BMP_DIR = $(PROTOCOL_DIR)/bmp
OPT_DEFS += -UNO_DEBUG

SRC += $(BMP_DIR)/gcc_startup_nrf52840.S
SRC += $(BMP_DIR)/bmp.c
SRC += $(BMP_DIR)/state_controller.c
SRC += $(BMP_DIR)/host_driver.c
SRC += $(BMP_DIR)/matrix/matrix.c
SRC += $(BMP_DIR)/matrix/matrix_basic.c
SRC += $(BMP_DIR)/matrix/matrix_duplex.c
SRC += $(BMP_DIR)/matrix/matrix_lpme.c
SRC += $(BMP_DIR)/matrix/lpme.c
SRC += $(BMP_DIR)/matrix/bmp_debounce.c
SRC += $(BMP_DIR)/cli/cli.c
SRC += $(BMP_DIR)/cli/microshell/core/microshell.c
SRC += $(BMP_DIR)/cli/microshell/core/mscore.c
SRC += $(BMP_DIR)/cli/microshell/util/mscmd.c
SRC += $(BMP_DIR)/cli/microshell/util/msopt.c
SRC += $(BMP_DIR)/cli/microshell/util/ntlibc.c

LDFLAGS += -L$(TMK_PATH)/$(BMP_DIR)

VPATH += $(TMK_PATH)/$(PROTOCOL_DIR)
VPATH += $(TMK_PATH)/$(BMP_DIR)
VPATH += $(TMK_PATH)/$(BMP_DIR)/matrix
VPATH += $(TMK_PATH)/$(BMP_DIR)/cli
VPATH += $(TMK_PATH)/$(BMP_DIR)/cli/microshell
VPATH += $(TMK_PATH)/$(BMP_DIR)/cli/microshell/core
VPATH += $(TMK_PATH)/$(BMP_DIR)/cli/microshell/util