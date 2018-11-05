#******************************************************************************
#
# Makefile - Rules for building cdl test.
#
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=
TARGET=test_cdl
TARGET_EXT=.axf

#
# The base directory for StellarisWare.
#
ROOT=.

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

# CFLAGS += -v

#
# Where to find source files that do not live in this directory.
#
VPATH=src

#
# Where to find header files that do not live in the source directory.
#
IPATH+=inc
IPATH+=$(ROOT)/libs/stdlib/include
IPATH+=$(ROOT)/libs/stdlib/inc
IPATH+=$(ROOT)/libs/tomcrypt/headers
IPATH+=$(ROOT)/libs/tomcrypt/mini-gmp
IPATH+=$(ROOT)/libs/tomcrypt
IPATH+=$(ROOT)/libs/usblib/include
IPATH+=$(ROOT)/drivers/include
IPATH+=$(ROOT)/../../cdl/include
IPATH+=$(ROOT)/../../cdl/inc

CSRCS = main.c int_handler.c test_utils.c \
				test_otp.c test_adc.c test_rng.c test_aes.c test_sha.c test_ipc.c\
				test_timer.c test_wdt.c test_rtc.c test_i2c.c test_dma.c  \
				test_gpio.c test_qspi.c test_uart.c test_spi.c test_mmc.c test_nand.c \
				test_usb.c test_crc.c test_ecc.c test_epf.c test_APIs.c cdl_tiny_fw.c \
				test_tamp.c test_pll.c test_ddr.c test_sys.c test_bburam.c test_dwc_trng.c 
CLIBS = \
				${ROOT}/libs/tomcrypt/${OUTPUT}/libtomcrypt.a \
				${ROOT}/drivers/${OUTPUT}/libdriver.a \
				${ROOT}/../../cdl/${OUTPUT}/libcdl.a \
				${ROOT}/libs/usblib/${OUTPUT}/libusb.a \
				${ROOT}/libs/usblib2/${OUTPUT}/libusb2.a \
				${ROOT}/libs/stdlib/${OUTPUT}/libstd.a

ifeq ($(BOARD_TYPE),HC)
SCATTER_FILE = cdl_ddr_hc.ld
endif

ifeq ($(BOARD_TYPE),HC-V2)
SCATTER_FILE = cdl_ddr_hc.ld
endif

ifeq ($(BOARD_TYPE),EVB)
SCATTER_FILE = cdl_ddr_evb.ld
endif

ifeq ($(BOARD_TYPE),EVB-V2)
SCATTER_FILE = cdl_ddr_evb.ld
endif

COBJS = $(addprefix ${OUTPUT}/, $(CSRCS:.c=.o))

#
# The default rule, which causes the USB host mass storage example to be built.
#
.PHONY: ${OUTPUT}
all: ${OUTPUT} ${OUTPUT}/$(TARGET)$(TARGET_EXT)

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${OUTPUT} ${wildcard *~}

#
# The rule to create the target directory.
#
${OUTPUT}:
	@mkdir -p ${OUTPUT}

#
# Rules for building the USB host mass storage example.
#
${OUTPUT}/$(TARGET)$(TARGET_EXT): $(OUTPUT)/start.o
${OUTPUT}/$(TARGET)$(TARGET_EXT): $(COBJS)
${OUTPUT}/$(TARGET)$(TARGET_EXT): $(CLIBS)
${OUTPUT}/$(TARGET)$(TARGET_EXT): $(SCATTER_FILE)
ifneq (SCATTER_FILE, )
SCATTERgcc_$(TARGET)=$(SCATTER_FILE)
endif
ENTRY_$(TARGET)=_start

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${OUTPUT}/*.d} __dummy__
endif
