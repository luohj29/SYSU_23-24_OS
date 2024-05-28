[bits 32]

global asm_hello_world

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