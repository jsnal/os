.intel_syntax noprefix

.section .text

.global context_run
context_run:
    mov eax, [esp + 4]
    mov esp, eax
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret

.global context_switch
context_switch:
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

    ret

// Artifically simulate an interrupt return by setting up the EFLAGS and CS
// register before this routine is called so 'iret' works as expected
.global context_switch_to_created
context_switch_to_created:
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
