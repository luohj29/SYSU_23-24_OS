my_id db "22336173_LHJ"; 
org 0x7c00
[bits 16]
; print"22336173" on the screen

set_cursor_first:
    mov bx, 0
    mov dh, 5   ; row5
    mov dl, 9; col8
    mov ah, 2
    int 10h
set_color:
    mov bl, 0x40    ; back red front black
    mov ecx, 12; loop 12 times
    mov si, my_id

print_loop:
    mov al, [si]; read a char from si address
    push cx; save the loop times
    mov cx, 0x0001; set number of char to print
    mov ah, 9
    int 10h
    call cursor_inc
    pop cx
    add si, 1; next char
    loop print_loop

    jmp $
    
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
