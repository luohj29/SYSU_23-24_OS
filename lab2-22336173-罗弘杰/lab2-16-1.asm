org 0x7c00
[bits 16]
; get cursor position
mov ah, 03h; get cursor position
mov bh, 0; page number
int 10h



; set cursor position
mov ah, 02h; set cursor position
mov bh, 0; page number
inc dh; increment row
inc dl; increment column
int 10h

jmp $

times 510-($-$$) db 0
dw 0x55, 0xaa