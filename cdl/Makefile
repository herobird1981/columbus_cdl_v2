#******************************************************************************
#
# Makefile - Rules for building the CDL library.
#
#
#******************************************************************************

#
# Defines the directory suffix that this project uses.
#
SUFFIX=
TARGET=libcdl.a

#
# The base directory
#
ROOT=../test/cdl

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=src

#
# Where to find header files that do not live in the source directory.
#
IPATH=inc
IPATH+=include
IPATH+=$(ROOT)/inc
IPATH+=${ROOT}/libs/stdlib/inc
IPATH+=${ROOT}/libs/stdlib/include
IPATH+=$(ROOT)/drivers/include

CSRCS = cdl_uart.c cdl_spi.c cdl_dmac.c cdl_wdt.c cdl_gpio.c cdl_i2c.c cdl_mmc.c cdl_gic.c \
	cdl_usb.c cdl_otp.c cdl_adc.c cdl_timer.c cdl_rtc.c cdl_crc.c cdl_sha.c cdl_rng.c \
	cdl_aes.c cdl_ipc.c cdl_tamp.c cdl_pll.c cdl_ddr.c cdl_sys.c cdl_bburam.c cdl_onfi.c cdl_nfc.c cdl_nfc_bbt.c cdl_bbt.c \
	qspi.c cdl_qspi_nand.c cdl_qspi_nand_bbt.c cdl_dwc_trng.c
	
COBJS = $(addprefix ${OUTPUT}/, $(CSRCS:.c=.o))

#
# The default rule
#
.PHONY: ${OUTPUT}
all: ${OUTPUT} ${OUTPUT}/$(TARGET)

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
# Rules for building the USB library.
#
${OUTPUT}/$(TARGET): $(COBJS)

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${OUTPUT}/*.d} __dummy__
endif
