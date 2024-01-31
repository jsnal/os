.intel_syntax noprefix

#define MAGIC           0x1BADB002
#define PAGE_ALIGN      (1 << 0)
#define MEMORY_INFO     (1 << 1)
#define FLAGS           (PAGE_ALIGN | MEMORY_INFO)
#define CHECKSUM        -(MAGIC+FLAGS)

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
