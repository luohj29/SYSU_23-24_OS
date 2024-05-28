[bits 32]

global asm_hello_world
global asm_lidt
global asm_unhandled_interrupt
global asm_halt

ASM_UNHANDLED_INTERRUPT_INFO db '222336173 interrupt happened, enter handle program'
                             db 0
ASM_IDTR dw 0
         dd 0

; void asm_unhandled_interrupt()
asm_unhandled_interrupt:
    cli
    mov esi, ASM_UNHANDLED_INTERRUPT_INFO
    xor ebx, ebx
    mov ah, 0x03
.output_information:
    cmp byte[esi], 0
    je .end
    mov al, byte[esi]
    mov word[gs:bx], ax
    inc esi
    add ebx, 2
    jmp .output_information
.end:
    jmp $

; void asm_lidt(uint32 start, uint16 limit)
asm_lidt:
    push ebp
    mov ebp, esp
    push eax

    mov eax, [ebp + 4 * 3]
    mov [ASM_IDTR], ax
    mov eax, [ebp + 4 * 2]
    mov [ASM_IDTR + 2], eax
    lidt [ASM_IDTR]

    pop eax
    pop ebp
    ret

asm_hello_world:
    push eax
    xor eax, eax

    mov ah, 0x03 ;ÇàÉ«
    mov al, 'K'
    mov [gs:2 * 0], ax

    mov al, 'E'
    mov [gs:2 * 1], ax

    mov al, 'R'
    mov [gs:2 * 2], ax

    mov al, 'N'
    mov [gs:2 * 3], ax

    mov al, 'E'
    mov [gs:2 * 4], ax

    mov al, 'L'
    mov [gs:2 * 5], ax

    mov al, ' '
    mov [gs:2 * 6], ax

    mov al, '2'
    mov [gs:2 * 7], ax

    mov al, '2'
    mov [gs:2 * 8], ax

    mov al, '3'
    mov [gs:2 * 9], ax

    mov al, '3'
    mov [gs:2 * 10], ax

    mov al, '6'
    mov [gs:2 * 11], ax

    mov al, '1'
    mov [gs:2 * 12], ax

    mov al, '7'
    mov [gs:2 * 13], ax

    mov al, '3'
    mov [gs:2 * 14], ax

    mov al, '_'
    mov [gs:2 * 15], ax

    mov al, 'L'
    mov [gs:2 * 16], ax

    mov al, 'H'
    mov [gs:2 * 17], ax

    mov al, 'J'
    mov [gs:2 * 18], ax
    pop eax
    ret

asm_halt:
    jmp $
