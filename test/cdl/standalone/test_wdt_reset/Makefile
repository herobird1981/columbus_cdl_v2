#******************************************************************************
#
# Makefile - Rules for building the standalone pattern
#
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=
TARGET=test_wdt
TARGET_EXT=.axf

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
VPATH=$(ROOT) $(ROOT)/test/cdl/src
#vpath $(ROOT)/start.S $(ROOT)/test/cdl/src

#
# Where to find header files that do not live in the source directory.
#
IPATH+=$(ROOT)/inc
IPATH+=$(ROOT)/libs/stdlib/include
IPATH+=$(ROOT)/libs/stdlib/inc
IPATH+=$(ROOT)/libs/tomcrypt/headers
IPATH+=$(ROOT)/libs/tomcrypt/mini-gmp
IPATH+=$(ROOT)/libs/tomcrypt
IPATH+=$(ROOT)/libs/usblib/include
IPATH+=$(ROOT)/drivers/include
IPATH+=$(ROOT)/../../cdl/include

CSRCS = main.c int_handler.c test_wdt.c
CLIBS = ${ROOT}/drivers/${OUTPUT}/libdriver.a \
				${ROOT}/../../cdl/${OUTPUT}/libcdl.a \
				${ROOT}/libs/stdlib/${OUTPUT}/libstd.a 
SCATTER_FILE = pattern.ld

COBJS = $(addprefix ${OUTPUT}/, $(CSRCS:.c=.o))

#
# The default rule, which causes the USB host mass storage example to be built.
#
.PHONY : ${OUTPUT} 
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
