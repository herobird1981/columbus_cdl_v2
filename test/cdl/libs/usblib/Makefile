#******************************************************************************
#
# Makefile - Rules for building the usblib
#
#
#******************************************************************************

#
# Defines the directory suffix that this project uses.
#
SUFFIX=
TARGET=libusb.a

#
# The base directory for StellarisWare.
#
ROOT=../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=src
VPATH+=musb-new
VPATH+=gadget
VPATH+=common
VPATH+=cmd
VPATH+=ulpi
VPATH+=disk

#
# Where to find header files that do not live in the source directory.
#
IPATH=inc
IPATH+=include
IPATH+=musb-new
IPATH+=$(ROOT)/../../cdl/include
IPATH+=$(ROOT)/libs/stdlib/inc
IPATH+=$(ROOT)/inc

CSRCS = usb_host_mass.c usb_dev_mass.c usb_port.c part.c\
	musb_gadget.c musb_gadget_ep0.c musb_host.c musb_core.c \
	musb_uboot.c ulpi.c ulpi-viewport.c \
	g_dnl.c f_thor.c f_dfu.c f_mass_storage.c f_fastboot.c \
	epautoconf.c config.c usbstring.c \
	ether.c rndis.c \
	usb_storage.c usb.c usb_hub.c \
	usb_mass_storage.c cmd_usb.c
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
