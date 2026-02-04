LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS := dev/usb

MODULE_SRCS += \
	$(LOCAL_DIR)/bunusatest.c

include make/module.mk
