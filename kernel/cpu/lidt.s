.global idt_flush
.global _exception0
.global isr_common

isr_common:
  pusha

  mov %ds, %ax
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
  sti
  iret

idt_flush:
  mov 4(%esp), %eax
  lidt (%eax)
  ret

_exception0:
  cli
  push $0x0
  push $0x0
  jmp isr_common

_exception1:
  cli
  push $0x0
  push $0x1
  jmp isr_common
