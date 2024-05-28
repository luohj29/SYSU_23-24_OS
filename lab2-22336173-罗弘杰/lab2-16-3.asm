org 0x7c00
[bits 16]

mov dh, 5   	; row 5
mov dl, 9		; col 8

kbIO:
mov ah, 0x00  ; function 1 of int 0x16 (keyboard input)
int 0x16
or al, 0x00   ; Test if the ASCII code of the key pressed is zero
jnz print_key ; If not zero, jump to print_key
jmp kbIO      ; If zero, continue waiting for input

print_key:
call cursor_inc

mov ah, 0x09  ; print character
mov bl, 0x40  ; color
mov cx, 0x01  ; number of characters
int 0x10
jmp kbIO      ; repeat

cursor_inc:
    inc dl
    cmp dl, 80
    jne add_cursor
    mov dl, 0
    inc dh
add_cursor:
    mov ah, 2; set cursor +1
    int 10h
    ret

times 510-($-$$) db 0
db 0x55, 0xaa
