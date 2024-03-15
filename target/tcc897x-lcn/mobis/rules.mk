LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)

ifeq ($(MOBIS_GET_DATA_FROM_MICOM), 1)
OBJS += \
        $(LOCAL_DIR)/mobis_getdata_micom.o
endif
