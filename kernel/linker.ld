ENTRY(boot)

SECTIONS
{
	/* Start the Boot code at 1 MB */
	. = 0x00100000;

	/* Read-only data for the Boot code */
        .boot.data : {
		*(.boot.data)
        }

	/* Text section for the Boot code */
	.boot.text : {
		*(.boot.text)
	}

	/* Start the Kernel at 3 GB */
	. += 0xC0000000;

        /* Text section */
	.text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	/* Read-write data */
	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	/* Read-write data and stack */
	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}

	/* Remember the end of the Kernel */
	kernel_end = .;
}
