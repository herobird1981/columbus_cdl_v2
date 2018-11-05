/*
 *Standard definitions of mode bits and interrupt (I & F) flags in PSRs
 */
#define Mode_USR	0x10
#define Mode_FIQ	0x11
#define Mode_IRQ	0x12
#define Mode_SVC	0x13
#define Mode_ABT	0x17
#define Mode_UND	0x1B
#define Mode_SYS	0x1F

#define I_Bit		0x80               // When I bit is set, IRQ is disabled
#define F_Bit		0x40               // When F bit is set, FIQ is disabled

/*
 *IRQ stack frame.
 */
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

/*
 * CR1 bits (CP#15 CR1)
 */
#define CR_M	(1 << 0)	/* MMU enable				*/
#define CR_A	(1 << 1)	/* Alignment abort enable		*/
#define CR_C	(1 << 2)	/* Dcache enable			*/
#define CR_W	(1 << 3)	/* Write buffer enable			*/
#define CR_P	(1 << 4)	/* 32-bit exception handler		*/
#define CR_D	(1 << 5)	/* 32-bit data address range		*/
#define CR_L	(1 << 6)	/* Implementation defined		*/
#define CR_B	(1 << 7)	/* Big endian				*/
#define CR_S	(1 << 8)	/* System MMU protection		*/
#define CR_R	(1 << 9)	/* ROM MMU protection			*/
#define CR_F	(1 << 10)	/* Implementation defined		*/
#define CR_Z	(1 << 11)	/* Implementation defined		*/
#define CR_I	(1 << 12)	/* Icache enable			*/
#define CR_V	(1 << 13)	/* Vectors relocated to 0xffff0000	*/
#define CR_RR	(1 << 14)	/* Round Robin cache replacement	*/
#define CR_L4	(1 << 15)	/* LDR pc can set T bit			*/
#define CR_DT	(1 << 16)
#define CR_IT	(1 << 18)
#define CR_ST	(1 << 19)
#define CR_FI	(1 << 21)	/* Fast interrupt (lower latency mode)	*/
#define CR_U	(1 << 22)	/* Unaligned access operation		*/
#define CR_XP	(1 << 23)	/* Extended page tables			*/
#define CR_VE	(1 << 24)	/* Vectored interrupts			*/
#define CR_EE	(1 << 25)	/* Exception (Big) Endian		*/
#define CR_TRE	(1 << 28)	/* TEX remap enable			*/
#define CR_AFE	(1 << 29)	/* Access flag enable			*/
#define CR_TE	(1 << 30)	/* Thumb exception enable		*/


.globl _start

/*
 *************************************************************************
 *
 * Vectors have their own section so linker script can map them easily
 *
 *************************************************************************
 */

	.section ".vectors", "ax"

/*
 *************************************************************************
 *
 * Exception vectors as described in ARM reference manuals
 *
 * Uses indirect branch to allow reaching handlers anywhere in memory.
 *
 *************************************************************************
 */

_start:

	b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

/*
 *************************************************************************
 *
 * Indirect vectors table
 *
 * Symbols referenced here must be defined somewhere else
 *
 *************************************************************************
 */

	.globl	_undefined_instruction
	.globl	_software_interrupt
	.globl	_prefetch_abort
	.globl	_data_abort
	.globl	_not_used
	.globl	_irq
	.globl	_fiq

_undefined_instruction:	.word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq

	.balignl 16,0xdeadbeef

/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */
	
	.macro get_irq_stack			@ setup IRQ stack; from .lds scripts
	ldr	sp, =_irq_stack
	.endm
	
	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	@ !!!! R8 NEEDS to be saved !!!! a reserved stack spot would be good.
	add	r8, sp, #S_PC
	stmdb	r8, {sp, lr}^		@ Calling SP, LR
	str	lr, [r8, #0]		@ Save calling PC
	mrs	r6, spsr
	str	r6, [r8, #4]		@ Save CPSR
	str	r0, [r8, #8]		@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	subs	pc, lr, #4		@ return & move spsr_svc into cpsr
	.endm
	
	.align	5
undefined_instruction:
software_interrupt:
prefetch_abort:
data_abort:
not_used:
fiq:

1:
	bl	1b			/* hang and never return */	

	.align	5
irq:
	get_irq_stack
	irq_save_user_regs
	bl	irq_generic_handler
	irq_restore_user_regs
	
/*************************************************************************
 *
 * Startup Code (reset vector)
 *
 *
 *************************************************************************/

	.globl	reset

reset:
	/*
	 * disable interrupts (FIQ and IRQ), also set the cpu to SVC32 mode,
	 * except if in HYP mode already
	 */
	mrs	r0, cpsr
	and	r1, r0, #0x1f		@ mask mode bits
	teq	r1, #0x1a		@ test for HYP mode
	bicne	r0, r0, #0x1f		@ clear all mode bits
	orrne	r0, r0, #0x13		@ set SVC mode
	orr	r0, r0, #0xc0		@ disable FIQ and IRQ
	msr	cpsr,r0

  /*
	* Initialize Supervisor Mode Stack
	* Note stack must be 8 byte aligned.
	* from .lds scripts
	*/
	ldr	sp, =_svc_stack
  
/*
 * Setup vector:
 */
	/* Set V=0 in CP15 SCTLR register - for VBAR to point to vector */
	mrc	p15, 0, r0, c1, c0, 0	@ Read CP15 SCTLR Register
	bic	r0, #CR_V		@ V = 0
	mcr	p15, 0, r0, c1, c0, 0	@ Write CP15 SCTLR Register

	/* Set vector address in CP15 VBAR register */
	ldr	r0, =_start
	mcr	p15, 0, r0, c12, c0, 0	@Set VBAR

	bl	cpu_init_cp15
	bl  clear_bss
	bl	main

/* returned from main, dead loop */
_exit:
	wfi
	b	_exit
/*************************************************************************
 *
 * cpu_init_cp15
 *
 * Setup CP15 registers (cache, MMU, TLBs). The I-cache is turned on unless
 * CONFIG_SYS_ICACHE_OFF is defined.
 *
 *************************************************************************/
cpu_init_cp15:

///////////
	@ Disable MMU.
	mrc p15, 0, r1, c1, c0, 0 								@ Read Control Register configuration data.
	bic r1, r1, #0x1
	mcr p15, 0, r1, c1, c0, 0 								@ Write Control Register configuration data.

	@ Disable L1 Caches.
	mrc p15, 0, r1, c1, c0, 0 								@ Read Control Register configuration data.
	bic r1, r1, #(0x1 << 12) 								@ Disable I Cache.
	bic r1, r1, #(0x1 << 2) 								@ Disable D Cache.
	mcr p15, 0, r1, c1, c0, 0 								@ Write Control Register configuration data

	@ Invalidate L1 Caches.
	@ Invalidate Instruction cache.
	mov r1, #0
	mcr p15, 0, r1, c7, c5, 0

	@ Invalidate Data cache.
	@ To make the code general purpose, calculate the
	@ cache size first and loop through each set + way.

	mrc p15, 1, r0, c0, c0, 0 								@ Read Cache Size ID.
	ldr r3, =0x1ff
	and r0, r3, r0, LSR #13 								@ r0 = no. of sets - 1.

	mov r1, #0 								@ r1 = way counter way_loop.
way_loop:
	mov r3, #0 								@ r3 = set counter set_loop.
set_loop:
	mov r2, r1, LSL #30
	orr r2, r3, LSL #5 								@ r2 = set/way cache operation format.
	mcr p15, 0, r2, c7, c6, 2								@ Invalidate the line described by r2.
	add r3, r3, #1 								@ Increment set counter.
	cmp r0, r3 								@ Last set reached yet?
	bgt set_loop 								@ If not, iterate set_loop,
	add r1, r1, #1 								@ else, next.
	cmp r1, #4 								@ Last way reached yet?
	bne way_loop 								@ if not, iterate way_loop.

	@ Invalidate TLB
	mcr p15, 0, r1, c8, c7, 0

	@ Branch Prediction Enable.
	mov r1, #0
	mrc p15, 0, r1, c1, c0, 0 								@ Read Control Register configuration data.
	orr r1, r1, #(0x1 << 11) 								@ Global BP Enable bit.
	mcr p15, 0, r1, c1, c0, 0

	/*
	 * Enable ACTLR.SMP for Cache to work.
	 */
	mrc	p15, 0, r0, c1, c0, 1
	mov	r0, #(1 << 6)
	mcr	p15, 0, r0, c1, c0, 1

#if 1
	@ Enable D-side Prefetch
	MRC p15, 0, r1, c1, c0, 1 								@ Read Auxiliary Control Register.
	ORR r1, r1, #(0x1 <<2) 								@ Enable D-side prefetch.
	MCR p15, 0, r1, c1, c0, 1;								@ Write Auxiliary Control Register.
	DSB
	ISB
	@ DSB causes completion of all cache maintenance operations appearing in program
	@ order before the DSB instruction.
	@ An ISB instruction causes the effect of all branch predictor maintenance
	@ operations before the ISB instruction to be visible to all instructions
	@ after the ISB instruction.
	@ Initialize PageTable.

	@ Create a basic L1 page table in RAM, with 1MB sections containing a flat
	@ (VA=PA) mapping, all pages Full Access, Strongly Ordered.

	@ It would be faster to create this in a read-only section in an assembly file.

	LDR r0, =0b00000000000000000000110111100010 												@ r0 is the non-address part of
 													@ descriptor.
	LDR r1, =0x10010000
	LDR r3, = 4095									@ loop counter.
write_pte:
	ORR r2, r0, r3, LSL #20									@ OR together address & default PTE bits.
	STR r2, [r1, r3, LSL #2]									@ Write PTE to TTB.
	SUBS r3, r3, #1									@ Decrement loop counter.
	BNE write_pte

	@ For the first entry in the table, You can make it cacheable, normal,	@ write-back, write allocate.
	BIC r0, r0, #0b1100 									@ Clear CB bits.
	ORR r0, r0, #0b0100 									@ inner write-back, write allocate
	BIC r0, r0, #0b111000000000000 									@ Clear TEX bits.
	ORR r0, r0, #0b101000000000000 									@ set TEX as write-back, write allocate
	ORR r0, r0, #0b10000000000000000 									@ shareable.
	STR r0, [r1]

	@ Initialize MMU.
	MOV r1,#0x0
	MCR p15, 0, r1, c2, c0, 2 								@ Write Translation Table Base Control Register.
	LDR r1, =0x10010000
	MCR p15, 0, r1, c2, c0, 0 								@ Write Translation Table Base Register 0.

	@ In this simple example, do not use TRE or Normal Memory Remap Register.
	@ Set all Domains to Client.
	LDR r1, =0x55555555
	MCR p15, 0, r1, c3, c0, 0 									@ Write Domain Access Control Register.

#endif

	dsb
	isb
	bx lr			@ back to my caller


.globl memcpy32_asm
.type memcpy32_asm, %function
memcpy32_asm:
	teq r0, r1
	beq memcpy32_asm_done
	add r2, r0, r2		@ end of copy
memcpy32_asm_next:
	ldr r3, [r1], #4
	str r3, [r0], #4
	cmp r0, r2
	blo memcpy32_asm_next
memcpy32_asm_done:
	dsb
	isb
	bx lr

.globl memset32_asm
.type memset32_asm, %function
memset32_asm:
	add r2, r0, r2
	teq r0, r2
	beq memset32_asm_done
memset32_asm_next:
	str r1, [r0], #4
	cmp r0, r2
	blo memset32_asm_next
memset32_asm_done:
	dsb
	isb
	bx lr


.globl copy_data
.type copy_data, %function
copy_data:
	ldr r0, =_data
	ldr r1, =_etext
	ldr r2, =_edata
	sub  r2, r2, r0

	mov ip, lr
	bl memcpy32_asm
	bx ip


.globl clear_bss
.type clear_bss, %function
clear_bss:
	ldr r0, =_bss
	ldr r2, =_ebss
	sub r2, r2, r0
	mov r1, #0

	mov ip, lr
	bl memset32_asm
	bx ip

.end
