.section .text
.global idt_flush
.global isr_common

isr_common:
  pusha

  mov %ds, %eax
  push %eax

  mov $0x10, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs

  call isr_handler

  pop %eax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs

  popa
  add $8, %esp
  iret

.macro isr_noerrcode num
  .global _exception\num
  _exception\num:
    push $0x0
    push $0x\num
    jmp isr_common
.endm

.macro isr_errcode num
  .global _exception\num
  _exception\num:
    push $0x\num
    jmp isr_common
.endm

idt_flush:
  mov 4(%esp), %eax
  lidt (%eax)
  ret

isr_noerrcode 0
isr_noerrcode 1
isr_noerrcode 2
isr_noerrcode 3
isr_noerrcode 4
isr_noerrcode 5
isr_noerrcode 6
isr_noerrcode 7
isr_errcode 8
isr_noerrcode 9
isr_errcode 10
isr_errcode 11
isr_errcode 12
isr_errcode 13
isr_errcode 14
isr_noerrcode 15
isr_noerrcode 16
isr_errcode 17
isr_noerrcode 18
isr_noerrcode 19
isr_noerrcode 20
isr_errcode 21
isr_noerrcode 22
isr_noerrcode 23
isr_noerrcode 24
isr_noerrcode 25
isr_noerrcode 26
isr_noerrcode 27
isr_noerrcode 28
isr_noerrcode 29
isr_noerrcode 30
isr_noerrcode 31
