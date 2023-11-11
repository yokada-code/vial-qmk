VIA_ENABLE = yes
VIAL_ENABLE = yes
VIAL_INSECURE = yes

SRC += quantizer_mouse.c

include keyboards/sekigon/keyboard_quantizer/mini/keymaps/vial/cli/rules.mk
include keyboards/sekigon/keyboard_quantizer/mini/keymaps/vial/key_override/rules.mk
VPATH += keyboards/sekigon/keyboard_quantizer/mini/keymaps/vial/cli
VPATH += keyboards/sekigon/keyboard_quantizer/mini/keymaps/vial/key_override

GIT_DESCRIBE := $(shell git describe --tags --long --dirty="\\*" 2>/dev/null)
CFLAGS += -DGIT_DESCRIBE=$(GIT_DESCRIBE)
