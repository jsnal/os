ENTRY(boot)

SECTIONS {
    /* Start the boot code at 1MB. */
    . = 0x00100000;

    /* Read-only data for the boot code. */
    .boot.data : {
        KEEP(*(.boot.data))
    }

    /* Text section for the boot code. */
    .boot.text : {
        KEEP(*(.boot.text))
    }

    /* Start the kernel at 3GB. */
    . += 0xC0000000;

    /* Text section. */
    .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.text)
    }

    /* Read-only data. */
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata)
    }

    /* Read-write data. */
    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
    }

    /* Read-write data and stack. */
    .bss ALIGN(4K) (NOLOAD) : AT(ADDR(.bss) - 0xC0000000)
    {
        *(COMMON)
        *(.bss)
        . = ALIGN(4K);
        *(.heap)
    }

    /* Remember the end of the kernel. */
    __end = ALIGN(4K);
}
