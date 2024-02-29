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
    mov edx, [esp + 24]

    mov [eax], esp
    mov esp, [edx]

    pop edi
    pop esi
    pop ebx
    pop ebp

    mov al, 0x20
    out 0x20, al
    sti
    ret
