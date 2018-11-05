#******************************************************************************
#
# Makefile - Columbus BSP makefile
#
#******************************************************************************
#default board: versatile express HC;  use make BOARD=EBV to build for EVB Board
#BOARD := HC

all: all-evb-v2

all-asic: BOARD := EVB
all-asic: all-common

all-fpga: BOARD := HC
all-fpga: all-common

all-fpga-v2: BOARD := HC-V2
all-fpga-v2: all-common

all-evb-v2: BOARD := EVB-V2
all-evb-v2: all-common

all-common: all-lib test

clean: clean-evb-v2

clean-asic: BOARD := EVB
clean-asic: clean-common

clean-fpga: BOARD := HC
clean-fpga: clean-common

clean-fpga-v2: BOARD := HC-V2
clean-fpga-v2: clean-common

clean-evb-v2: BOARD := EVB-V2
clean-evb-v2: clean-common

clean-common: all-lib-clean test-clean

## cdl ##
.PHONY: cdl
cdl:
	+make -C cdl BOARD_TYPE=$(BOARD)
cdl-clean:
	+@make -C cdl clean BOARD_TYPE=$(BOARD)

## drivers ##
drivers:
	+@make -C test/cdl/drivers BOARD_TYPE=$(BOARD)
drivers-clean:
	+@make -C test/cdl/drivers clean BOARD_TYPE=$(BOARD)

## stdlib ##
stdlib:
	+@make -C test/cdl/libs/stdlib BOARD_TYPE=$(BOARD)
stdlib-clean:
	+@make -C test/cdl/libs/stdlib clean BOARD_TYPE=$(BOARD)

## tomcrypt lib ##
tomcrypt:
	+@make -C test/cdl/libs/tomcrypt BOARD_TYPE=$(BOARD)
tomcrypt-clean:
	+@make -C test/cdl/libs/tomcrypt clean BOARD_TYPE=$(BOARD)

## usb lib ##
usblib:
	+@make -C test/cdl/libs/usblib BOARD_TYPE=$(BOARD)
usblib-clean:
	+@make -C test/cdl/libs/usblib clean BOARD_TYPE=$(BOARD)

## usb lib2 ##
usblib2:
	@make -C test/cdl/libs/usblib2 BOARD_TYPE=$(BOARD)
usblib2-clean:
	@make -C test/cdl/libs/usblib2 clean BOARD_TYPE=$(BOARD)

## all libs ##
all-lib: stdlib tomcrypt usblib usblib2 cdl drivers

all-lib-clean: stdlib-clean tomcrypt-clean usblib-clean usblib2-clean cdl-clean drivers-clean

## cdl test ##
.PHONY: test
test: all-lib
	+@make -C test/cdl BOARD_TYPE=$(BOARD)
test-clean: all-lib-clean
	+@make -C test/cdl clean BOARD_TYPE=$(BOARD)




