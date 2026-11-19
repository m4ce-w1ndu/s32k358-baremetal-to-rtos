/* Linker script for FW startup on S32K358 */

/* Entry point */
ENTRY(Reset_Handler)

/* Memory block addresses range */
MEMORY
{
	FLASH (rx) : ORIGIN = 0x00400000, LENGTH = 8M
	SRAM (rwx) : ORIGIN = 0x20400000, LENGTH = 768K
}

/* Symbols to export */
_sram_start = ORIGIN(SRAM);
_sram_end = ORIGIN(SRAM) + LENGTH(SRAM);
_estack = (_sram_end & ~7);

/* Sections */
SECTIONS
{
	.isr_vector :
	{
		. = ALIGN(512);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} > FLASH

	.text :
	{
		. = ALIGN(4);
		*(.text)
		*(.text*)
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4);
	} > FLASH

	.data :
	{
		. = ALIGN(4);
		_sdata = .;
		*(.data)
		*(.data*)
		. = ALIGN(4);
		_edata = .;
	} > SRAM AT > FLASH

	_sidata = LOADADDR(.data);

	.bss (NOLOAD) :
	{
		. = ALIGN(4);
		_sbss = .;
		*(.bss)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
	} > SRAM
}
