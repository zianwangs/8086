OUTPUT_FORMAT("binary")
ENTRY(main)

SECTIONS
{
	/* Link the kernel at this address: "." means the current address */
        /* Must be equal to KERNLINK */
	. = 0x80000000;

	.text : { *(.text) }
	/* .rodata : { *(.rodata) } */
	. = ALIGN(0x10);
    .data : { *(.data) }
	/* .bss : { *(.bss) } */

}
