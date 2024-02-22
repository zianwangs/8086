OUTPUT_FORMAT("binary")
/* OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64") */
ENTRY(main)

SECTIONS
{
	/* Link the kernel at this address: "." means the current address */
		/* Must be equal to KERNLINK */
	. = 0x80000000;

	.text : { *(.text) }
	. = ALIGN(0x10);
	.rodata : { *(.rodata) }
	. = ALIGN(0x10);
	.data : { *(.data) }
	. = ALIGN(0x10);
	.bss : { *(.bss) }
	

}
