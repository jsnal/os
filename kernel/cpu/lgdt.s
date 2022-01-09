.section .text
.global gdt_flush

gdt_flush:
  mov 4(%esp), %eax
  lgdt (%eax)

  mov $0x10, %eax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs
  mov %ax, %ss
  jmp $0x8,$.flush
.flush:
  ret
