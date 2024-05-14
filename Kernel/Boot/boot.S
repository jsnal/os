.intel_syntax noprefix

.set MAGIC, 0x1BADB002
.set PAGE_ALIGN, (1 << 0)
.set MEMORY_INFO, (1 << 1)
.set FLAGS, (PAGE_ALIGN | MEMORY_INFO)
.set CHECKSUM, -(MAGIC+FLAGS)

# Declare the Multiboot header
.section .boot.data, "aw"
    .align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .bss, "aw", @nobits
    # Allocate initial stack space
    .align 16
    stack_bottom:
    .skip 16384
    stack_top:

    # Allocate page directory and two page tables
    .align 4096
    boot_page_directory:
    .skip 4096
    identity_mapped_boot_page_table:
    .skip 4096
    boot_page_table:
    .skip 4096

# Boot entry point
.section .boot.text, "a"
.global boot
.type boot, @function
boot:
    cli

    # Prepare to populate the boot page table
    mov edi, offset (boot_page_table - 0xC0000000)
    mov esi, 0x3
    mov ecx, 1024

    boot_page_table_setup:
        # Write 4096 page entries into the boot page table
        mov [edi], esi
        add edi, 4
        add esi, 4096
        loop boot_page_table_setup

    # Prepare to populate the identity mapped boot page table
    mov edi, offset (identity_mapped_boot_page_table - 0xC0000000)
    mov esi, 0x3
    mov ecx, 1024

    identity_mapped_boot_page_table_setup:
        # Write 4096 page entries into the identity mapped boot page table
        mov [edi], esi
        add edi, 4
        add esi, 4096
        loop identity_mapped_boot_page_table_setup

    # Map the identity mapped boot page table to virtual addresses 0x00000000-0x003fffff
    mov ecx, offset ((identity_mapped_boot_page_table - 0xC0000000) + 3)
    mov [boot_page_directory - 0xC0000000], ecx

    # Map the boot page table to virtual addresses 0xc0000000-0xc03fffff
    mov ecx, offset ((boot_page_table - 0xC0000000) + 3)
    mov [boot_page_directory - 0xC0000000 + 768 * 4], ecx

    # Load the page directory
    mov ecx, offset (boot_page_directory - 0xC0000000)
    mov cr3, ecx

    # Enable paging
    mov ecx, cr0
    or ecx, 0x80000001
    mov cr0, ecx

    # Jump to high-half Kernel
    lea ecx, kernel
    jmp ecx

# Kernel entry point
.section .text
kernel:
    # Setup the stack
    mov esp, offset stack_top
    mov ebp, esp

    # Push the multiboot headers
    push eax
    push ebx

    # Push the address of the boot page table
    push offset boot_page_directory

    # Call the C code entry point
    call kernel_entry

    cli
    hang:
        hlt
        jmp hang

