#******************************************************************************
#
# Makefile - Rules for building the drviers
#
#
#******************************************************************************

#
# Defines the directory suffix that this project uses.
#
SUFFIX=
TARGET=libdriver.a

#
# The base directory for StellarisWare.
#
ROOT=..

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
IPATH+=$(ROOT)/../../cdl/include

CSRCS = console.c interrupt.c timer.c spi_flash.c mmc.c
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
