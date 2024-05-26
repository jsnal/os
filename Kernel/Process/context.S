.intel_syntax noprefix

.section .text

.global start_kernel_process
start_kernel_process:
    mov eax, [esp + 4]
    mov ebx, [esp + 8]

    mov cr3, ebx
    mov esp, eax

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

.global context_run
context_run:
    mov eax, [esp + 4]
    mov esp, eax
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret

.global do_context_switch
do_context_switch:
    pushad
    pushfd
    push gs
    push fs
    push es
    push ds

    xchg bx, bx

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

// Artifically simulate an interrupt return by setting up the EFLAGS and CS
// register before this routine is called so 'iret' works as expected
.global context_switch_to_created
context_switch_to_created:
    // xchg bx, bx
    push ebp
    push ebx
    push esi
    push edi

    mov eax, [esp + 20]
    mov ecx, [esp + 24]
    mov edx, [esp + 28]

    mov [eax], esp
    mov cr3, edx
    mov esp, [ecx]

    pop edi
    pop esi
    pop ebx
    pop ebp

    mov al, 0x20
    out 0x20, al
    iret
