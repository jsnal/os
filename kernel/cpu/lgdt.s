.global gdt_flush
.global gdt_reload

gdt_flush:
  mov 4(%esp), %eax
  lgdt (%eax)
  mov $0x10, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs
  mov %ax, %ss
  jmp $0x08,$.flush
.flush:
  ret

gdt_reload:
  jmp $0x08,$.reload
.reload:
  mov $0x10, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs
  mov %ax, %ss
  ret
