#******************************************************************************
#
# Makefile - Rules for building the usblib2
#
#
#******************************************************************************

#
# Defines the directory suffix that this project uses.
#
SUFFIX=
TARGET=libusb2.a

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
VPATH=src/cd
VPATH+=src/drivers/msd
VPATH+=src/examples/msd
VPATH+=src/lib
VPATH+=src/systems/none
VPATH+=src/systems/none/afs
VPATH+=src/test/msd

#
# Where to find header files that do not live in the source directory.
#
IPATH=include
IPATH+=include/class
IPATH+=src/cd
IPATH+=src/drivers/msd
IPATH+=src/systems/none
IPATH+=src/systems/none/afs
IPATH+=$(ROOT)/libs/stdlib/inc
IPATH+=$(ROOT)/drivers/include
IPATH+=$(ROOT)/inc
IPATH+=$(ROOT)/../../cdl/inc

CSRCS = \
	mu_cntlr.c \
	mu_drc.c \
	mu_hdr.c \
	mu_list.c \
	mu_pip.c \
	mu_descs.c \
	mu_fun.c \
	mu_hst.c \
	mu_mdr.c \
	mu_8070i.c \
	mu_mapi.c \
	mu_mbot.c \
	mu_mscsi.c \
	mu_msdfn.c \
	mu_bits.c \
	mu_ramfl.c \
	mu_stack.c \
	mu_stdio.c \
	mu_strng.c \
	plat_uds.c \
	board.c \
	tst_msd.c

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
