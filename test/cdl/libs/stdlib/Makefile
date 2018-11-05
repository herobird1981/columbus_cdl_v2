#******************************************************************************
#
# Makefile - Rules for building the stdlib.
#
#
#******************************************************************************

#
# Defines the directory suffix that this project uses.
#
SUFFIX=
TARGET=libstd.a

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

#
# Where to find header files that do not live in the source directory.
#
IPATH=inc
IPATH+=include

CSRCS = qsort.c string.c eabi_compat.c ctype.c div64.c vsprintf.c strto.c \
	crc32.c cmdline.c rand.c dlmalloc.c panic.c linux_compat.c utils.c memleak.c
ASMSRCS = _divsi3.S _udivsi3.S
COBJS = $(addprefix ${OUTPUT}/, $(CSRCS:.c=.o))
ASMOBJS = $(addprefix ${OUTPUT}/, $(ASMSRCS:.S=.o))

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
${OUTPUT}/$(TARGET): $(COBJS) $(ASMOBJS)

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${OUTPUT}/*.d} __dummy__
endif
