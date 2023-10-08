PROTOCOL_DIR = protocol
BMP_DIR = $(PROTOCOL_DIR)/bmp

SRC += $(BMP_DIR)/bmp.c
SRC += $(BMP_DIR)/matrix/matrix.c

LDFLAGS += -L$(BMP_DIR)

VPATH += $(TMK_PATH)/$(PROTOCOL_DIR)
VPATH += $(TMK_PATH)/$(BMP_DIR)