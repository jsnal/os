.intel_syntax noprefix

.section .text

.global start_kernel_process
start_kernel_process:
    mov esp, [esp + 4]

    pop ds
    pop es
    pop fs
    pop gs

    popfd
    popad

    ret

.global start_user_process
start_user_process:
    mov esp, [esp + 4]
    iret

.global do_context_switch
do_context_switch:
    pushad
    pushfd
    push gs
    push fs
    push es
    push ds

    // Save the old stack pointer
    mov eax, [esp + 56]
    mov [eax], esp

    // Set the new CR3 register
    mov eax, [esp + 64]
    mov cr3, eax

    // Set the ESP for the new proccess
    mov esp, [esp + 60]

    pop ds
    pop es
    pop fs
    pop gs
    popfd
    popad

    ret
