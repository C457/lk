# top level project rules for the tcc897x_evm project
#
LOCAL_DIR := $(GET_LOCAL_DIR)

TARGET := tcc897x-lcn

MODULES += app/aboot

DEBUG := 1
#DEFINES += WITH_DEBUG_DCC=1
DEFINES += WITH_DEBUG_UART=1
#DEFINES += WITH_DEBUG_FBCON=1
DEFINES += DEVICE_TREE=1
#DEFINES += MMC_BOOT_BAM=1
#DEFINES += CRYPTO_BAM=1

#Disable thumb mode
ENABLE_THUMB := false

TCC897X := 1

ifeq ($(NAND_BOOT),1)
DEFINES += _NAND_BOOT=1
else
EMMC_BOOT := 1
ifeq ($(EMMC_BOOT),1)
DEFINES += _EMMC_BOOT=1
endif
endif

ifeq ($(HDEN),1)
HIDDEN_ENABLE := 1
endif

ifeq ($(GPHEN),1)
GPHD_ENABLE := 1
endif

TZOW_ENABLE := 1
TSBM_INCLUDE := 1
DEFINES += TSBM_ENABLE

#TCC_NSK_ENABLE := 1
#NSK_MP := 1
ifeq ($(TCC_NSK_ENABLE),1)
	DEFINES += TCC_NSK_ENABLE
ifeq ($(NSK_MP),1)
	DEFINES += NSK_MP
endif
endif

DEFINES += TCC_KPANIC=1

DAUDIO := 1

ifeq ($(DAUDIO),1)
	TCC_SPLASH_USE := 1
	DAUDIO_VER := 1
ifeq ($(DAUDIO_VER),1)
	DEFINES += DAUDIO_VER=1
endif
endif
