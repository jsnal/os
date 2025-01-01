.intel_syntax noprefix

.section .text

.global start_first_process
start_first_process:
    mov esp, [esp + 4]

    pop ebp
    pop edi
    pop esi
    pop ebx
    popfd

    ret

.global first_context_switch
first_context_switch:
    pop gs
    pop fs
    pop es
    pop ds
    pop esi
    pop edi
    pop ebp
    pop edx
    pop ecx
    pop ebx
    pop eax

    iret

.global context_switch
context_switch:
    pushfd
    push ebx
    push esi
    push edi
    push ebp

    // Save the old stack pointer
    mov eax, [esp + 24]
    mov [eax], esp

    // Set the new CR3 register
    mov eax, [esp + 32]
    mov cr3, eax

    // Set the ESP for the new proccess
    mov esp, [esp + 28]

    pop ebp
    pop edi
    pop esi
    pop ebx
    popfd

    ret
