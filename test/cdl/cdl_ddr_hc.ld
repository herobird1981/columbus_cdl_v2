/******************************************************************************
 *
 * cdl_ddr.ld - Linker configuration file for cdl test.
 *
 *
 *****************************************************************************/

ddr_start = 0x43F80000;
ddr_end = 0x48000000;
ddr_size = ddr_end - ddr_start;

MEMORY
{
	/* SRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0x00020000 */
	DDR (rwx) : ORIGIN = ddr_start, LENGTH = ddr_size
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

    .data : ALIGN(8) /* AT(ADDR(.text) + SIZEOF(.text)) */
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

    _bss = .;
    .bss : ALIGN(8) {
        *(.bss*)
        /* *(COMMON) */
    } > DDR
    
    .stack (NOLOAD) : ALIGN(8) {
        _esvc_stack = .;
        . += 0x40000;  /* 256 KB */
        _svc_stack = .;
        
        . = ALIGN(8);
        _eirq_stack = .;
        . += 0x10000;	/* 64 KB */
        _irq_stack = .;
        
        . = ALIGN(4);
     } > DDR
     _ebss = .;
    
	_malloc = .;
    .malloc (NOLOAD) : ALIGN(16) {
    	. += 0x2000000;  /* 32 MB */
    } > DDR
    _emalloc = .;
    
	/DISCARD/ : {
		*(.ARM.exidx*)
		*(.dynsym)
		*(.ARM.extab*)
		*(.interp*)
		*(.dynbss)
		*(.dynstr*)
		*(.dynamic*)
		*(.plt*)
		*(.gnu.hash)
		*(.gnu*)
		*(.ARM.exidx*)
		*(.gnu.linkonce.armexidx.*)
	}

}
