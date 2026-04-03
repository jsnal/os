MAGIC equ 0x1BADB002
PAGE_ALIGN equ (1 << 0)
MEMORY_INFO equ (1 << 1)
FLAGS equ (PAGE_ALIGN | MEMORY_INFO)
CHECKSUM equ -(MAGIC + FLAGS)

; Declare the Multiboot header
section .boot.data
    align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
    ; Allocate initial stack space (16 KB)
    align 16
    stack_bottom:
    resb 16384
    stack_top:

    ; Allocate page directory and two page tables (4 KB each)
    align 4096
    boot_page_directory:
    resb 4096
    identity_mapped_boot_page_table:
    resb 4096
    boot_page_table:
    resb 4096

section .boot.text
global boot
boot:
    cli

    ; Prepare to populate the boot page table
    mov edi, (boot_page_table - 0xC0000000)
    mov esi, 3
    mov ecx, 1024

boot_page_table_setup:
    ; Write 1024 page entries (mapping 4MB)
    mov [edi], esi
    add edi, 4
    add esi, 4096
    loop boot_page_table_setup

    ; Prepare to populate the identity mapped boot page table
    mov edi, (identity_mapped_boot_page_table - 0xC0000000)
    mov esi, 3
    mov ecx, 1024

identity_mapped_boot_page_table_setup:
    ; Write 1024 page entries into the identity mapped boot page table (mapping 4MB)
    mov [edi], esi
    add edi, 4
    add esi, 4096
    loop identity_mapped_boot_page_table_setup

    ; Map the identity mapped boot page table to 0x00000000-0x003fffff
    mov ecx, (identity_mapped_boot_page_table - 0xC0000000 + 3)
    mov [boot_page_directory - 0xC0000000], ecx

    ; Map the boot page table to 0xc0000000-0xc03fffff (Entry 768)
    mov ecx, (boot_page_table - 0xC0000000 + 3)
    mov [boot_page_directory - 0xC0000000 + 768 * 4], ecx

    ; Load the page directory into CR3
    mov ecx, (boot_page_directory - 0xC0000000)
    mov cr3, ecx

    ; Enable paging (set PG and PE bits in CR0)
    mov ecx, cr0
    or ecx, 0x80000001
    mov cr0, ecx

    ; Jump to high-half Kernel
    lea ecx, [kernel]
    jmp ecx
    hlt

section .text
kernel:
    ; Setup the stack
    mov esp, stack_top
    mov ebp, esp

    ; Push the multiboot headers
    push eax
    push ebx

    ; Push the address of the boot page table
    push boot_page_directory

    ; Call the Rust code entry point
    mov dword [0xb8000], 0x2f4b2f4f
    ; extern kernel_entry
    ; call kernel_entry

    cli
hang:
    hlt
    jmp hang
