/******************************************************************************
 *
 * cdl_ddr.ld - Linker configuration file for cdl test.
 *
 *
 *****************************************************************************/

MEMORY
{
    SRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0x00020000
    DDR (rwx) : ORIGIN = 0x00000000, LENGTH = 0x01000000
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > DDR

    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        _data = .;
        *(vtable)
        *(.data*)
        _edata = .;
    } > DDR

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        _esvc_stack = .;
        . += 0x1000;
        _svc_stack = .;
        . = ALIGN(8);
        _eirq_stack = .;
        . += 0x100;
        _irq_stack = .;
        _ebss = .;
    } > DDR
}
