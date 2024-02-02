.intel_syntax noprefix

.set MAGIC, 0x1BADB002
.set PAGE_ALIGN, (1 << 0)
.set MEMORY_INFO, (1 << 1)
.set FLAGS, (PAGE_ALIGN | MEMORY_INFO)
.set CHECKSUM, -(MAGIC+FLAGS)

.section .multiboot
    .align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .bss
    .align 16
    stack_bottom:
    .skip 16384
    stack_top:

.section .text
.global _start
.type _start, @function
_start:
    mov esp, OFFSET stack_top
    mov ebp, esp

    push eax
    push ebx
    call bootloader_entry

    cli
    hang:
        hlt
        jmp hang

.size _start, . - _start
