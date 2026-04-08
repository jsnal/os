.section .text

// Do not pass error code back
.altmacro
.macro ISR_NO_ERROR num
isr_\num:
    cli
    push 0x0
    push \num
    jmp isr_common
.endm

// Pass error code back
.altmacro
.macro ISR_ERROR num
isr_\num:
    cli
    push \num
    jmp isr_common
.endm

.global isr_common
isr_common:
    pusha
    push ds
    push es
    push fs
    push gs

    // Load the Kernel's data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call isr_handler
    add esp, 0x4

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 0x08
    iret

ISR_NO_ERROR 0
ISR_NO_ERROR 1
ISR_NO_ERROR 2
ISR_NO_ERROR 3
ISR_NO_ERROR 4
ISR_NO_ERROR 5
ISR_NO_ERROR 6
ISR_NO_ERROR 7
ISR_ERROR 8
ISR_ERROR 10
ISR_ERROR 11
ISR_ERROR 12
ISR_ERROR 13
ISR_ERROR 14
ISR_NO_ERROR 16
ISR_ERROR 17
ISR_NO_ERROR 18
ISR_NO_ERROR 19
ISR_NO_ERROR 20
ISR_ERROR 21
ISR_NO_ERROR 28
ISR_ERROR 29
ISR_ERROR 30
ISR_NO_ERROR 31

.set i, 32
.rept 224
    ISR_NO_ERROR %i
    .set i, i + 1
.endr

.global isrs
isrs:
    .long isr_0
    .long isr_1
    .long isr_2
    .long isr_3
    .long isr_4
    .long isr_5
    .long isr_6
    .long isr_7
    .long isr_8
    .long 0
    .long isr_10
    .long isr_11
    .long isr_12
    .long isr_13
    .long isr_14
    .long 0
    .long isr_16
    .long isr_17
    .long isr_18
    .long isr_19
    .long isr_20
    .long isr_21
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long isr_28
    .long isr_29
    .long isr_30
    .long isr_31

    .altmacro
    .macro GEN num
        .long isr_\num
    .endm

    .set i, 32
    .rept 224
        GEN %i
        .set i, i + 1
    .endr
