.intel_syntax noprefix

.section .text

.global idt_load
.global isr_common

.macro ISR num
.global isr_\num


// Do not pass error-code back
isr_\num:
    cli
    push 0x0
    push \num
    jmp isr_common
.endm

// Pass error-code back
.macro ISR_ERROR num
.global isr_\num

isr_\num:
    cli
    push \num
    jmp isr_common
.endm

idt_load:
    lidt [eax]
    ret

isr_common:
    pusha // Save edi, esi, ebp, esp, edx, ecx, eax to the stack

    push ds // Save the data segment

    mov ax, 0x10 // Load the kernel's data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp // ?

    call isr_handler

    add esp, 0x4 // ?

    pop eax // Load the original data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa // Restore edi, esi, ebp, esp, edx, ecx, eax
    add esp, 0x8 // Move past the error code and isr number
    iret

ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR_ERROR 8
ISR 9
ISR_ERROR 10
ISR_ERROR 11
ISR_ERROR 12
ISR_ERROR 13
ISR_ERROR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
ISR 20
ISR 21
ISR 22
ISR 23
ISR 24
ISR 25
ISR 26
ISR 27
ISR 28
ISR 29
ISR 30
ISR 31
ISR 32
ISR 33
ISR 34
ISR 35
ISR 36
ISR 37
ISR 38
ISR 39
ISR 40
ISR 41
ISR 42
ISR 43
ISR 44
ISR 45
ISR 46
ISR 47
