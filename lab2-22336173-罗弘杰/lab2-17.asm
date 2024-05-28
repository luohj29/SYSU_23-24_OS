[bits 32]
%include "head.include"

    mov eax, [a1]
    mov ebx, [a2]
your_if:
    cmp eax, 12
    jl lt12 ; if eax < 12 then jump to lt12
    cmp eax, 24
    jl lt24 ; if eax < 24 then jump to lt24
    shl eax, 4; a1*=16
    mov [if_flag], eax; change if_flag to a1
    jmp your_while

lt24:
    mov ecx, eax ; ecx = eax
    sub ecx, 24 ; ecx = ecx - 24
    neg ecx ; ecx = -ecx
    imul ecx, eax; ecx = ecx * eax
    mov [if_flag], ecx
    jmp your_while

lt12:
    sar eax, 1 ; a1/=2
    inc eax ; a1++
    mov [if_flag], eax
    jmp your_while

your_while:
    cmp dword[a2], 12
    jl lt_12; if ebx < 12 then jump to lt12
    call my_random
    mov ebx, [a2]
    mov dword[ecx + ebx - 12], eax
    dec dword[a2]
    jmp your_while

lt_12:
%include "end.include"

your_function:
    pushad
    mov eax, 0
loop:
    mov ecx, [your_string]
    cmp byte[ecx+eax], 0
    je loopend
    pushad
    mov ebx, dword[ecx+eax]
    push ebx
    call print_a_char
    pop ebx
    popad
    add eax, 1;
    jmp loop;

loopend:
    popad
    ret