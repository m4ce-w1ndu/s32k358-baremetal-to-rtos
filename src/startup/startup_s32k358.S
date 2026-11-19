// Set unified ARM assembly syntax and proper CPU type
.syntax unified
.cpu cortex-m7
.thumb

// Interrupt vector table section
.section .isr_vector, "a", %progbits

// Basic vector table definition
.global g_pfnVectors
g_pfnVectors:
	.word _estack
	.word Reset_Handler
	.word NMI_Handler
	.word HardFault_Handler
	.word MemManage_Handler
	.word BusFault_Handler
	.word UsageFault_Handler
	.word 0, 0, 0, 0
	.word SVC_Handler
	.word DebugMon_Handler
	.word 0
	.word PendSV_Handler
	.word SysTick_Handler

// Reset handler definition
.section .text.Reset_Handler
.global Reset_Handler
.type Reset_Handler, %function
.thumb_func
Reset_Handler:
	cpsid i
	mov r2, #0
	mov r3, #0
	
	// Initialize ECC SRAM
	ldr r0, =_sram_start
	ldr r1, =_sram_end
	
	ecc_sram_init:
		cmp r0, r1
		bhs ecc_sram_init_end
		strd r2, r3, [r0], #8
		b ecc_sram_init
	ecc_sram_init_end:

	// Setup MSP
	ldr r0, =_estack
	msr msp, r0
	
	// Copy _sidata to .data
	ldr r0, =_sidata
	ldr r3, =_sdata
	ldr r4, =_edata

	sidata_to_data:
		cmp r3, r4
		bhs sidata_to_data_end

		ldr r2, [r0], #4
		str r2, [r3], #4
		b sidata_to_data
	sidata_to_data_end:

	// Zero bss area
	ldr r0, =_sbss
	ldr r1, =_ebss
    mov r2, #0
    
	bss_init:
		cmp r0, r1
		bhs bss_init_end
	
		str r2, [r0], #4
		b bss_init
	bss_init_end:

	// Set SCB->VTOR (0xE000ED08) to g_pfnVectors
    ldr r0, =0xE000ED08
    ldr r1, =g_pfnVectors

	str r1, [r0]
    dsb
    isb
	
	// Unmask all interrupts
	cpsie i

	// call main
	bl main

    b .

// Default handler definition
.section .text.Default_Handler
.global Default_Handler
.type Default_Handler, %function
.thumb_func
Default_Handler:
    b .

// Add weak and thumb aliases for handlers
.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler
.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler
.weak BusFault_Handler
.thumb_set BusFault_Handler, Default_Handler
.weak UsageFault_Handler
.thumb_set UsageFault_Handler, Default_Handler
.weak SVC_Handler
.thumb_set SVC_Handler, Default_Handler
.weak DebugMon_Handler
.thumb_set DebugMon_Handler, Default_Handler
.weak PendSV_Handler
.thumb_set PendSV_Handler, Default_Handler
.weak SysTick_Handler
.thumb_set SysTick_Handler, Default_Handler
.weak MemManage_Handler
.thumb_set MemManage_Handler, Default_Handler
