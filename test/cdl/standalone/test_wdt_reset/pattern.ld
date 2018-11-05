/******************************************************************************
 *
 * cdl_ddr.ld - Linker configuration file for cdl test.
 *
 *
 *****************************************************************************/

MEMORY
{
	SRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0x00020000
	DDR (rwx) : ORIGIN = 0x00000000, LENGTH = 0x00020000
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
	. = ALIGN(4);
        _etext = .;
    } > DDR

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        _data = .;
        *(vtable)
        *(.data*)
    } > DDR

    . = ALIGN(4);
    .u_boot_list : {
	KEEP(*(SORT(.u_boot_list*)));
    } > DDR
    . = ALIGN(4);
    _edata = .;

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        _esvc_stack = .;
        . += 0x10000;
        _svc_stack = .;
        . = ALIGN(8);
        _eirq_stack = .;
        . += 0x1000;
        _irq_stack = .;
	. = ALIGN(4);
        _ebss = .;
    } > DDR
}
