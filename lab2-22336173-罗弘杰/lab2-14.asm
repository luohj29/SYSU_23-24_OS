org 0x7c00

_DR equ 1
_UR equ 2
_UL equ 3
_DL equ 4
delay equ 2000
ddelay equ 100


START:
    mov ax, cs
    mov es, ax
    mov ds, ax
    mov ax, 0b800h
    mov es, ax
    mov si, 0
    mov di, 0

PRINT1:
    mov bx, name    
    mov al, [bx+si]
    cmp al, 0
    jz Loop    
    mov bx, 52
    mov byte[es:bx+di], al    
    mov byte[es:bx+di+1], 1
    inc si
    add di, 2
    jmp PRINT1

    mov si, name
Loop:
    dec word[count]
    jnz Loop

    mov word[count], delay
    dec word[dcount]
    jnz Loop

    mov word[count], delay
    mov word[dcount], ddelay

    mov al,1
    cmp al, byte[rdul]
    je D_R

    mov al,2
    cmp al, byte[rdul]
    je U_R

    mov al,3
    cmp al, byte[rdul]
    je U_L

    mov al,4
    cmp al, byte[rdul]
    je D_L

    jmp $

D_R:
    inc byte[x]
    inc byte[y]

    mov bh, byte[x]
    mov bl, 25
    cmp bh, bl
    je dr2ur

    mov bh, byte[y]
    mov bl, 80
    cmp bh, bl
    je dr2dl

    jmp display

dr2ur:
    mov byte[x], 23
    mov byte[rdul], _UR
    jmp display

dr2dl:
    mov byte[y], 78
    mov byte[rdul], _DL
    jmp display

D_L:
    inc byte[x]
    dec byte[y]

    mov bh, byte[x]
    mov bl, 25
    cmp bh, bl
    je dl2ul

    mov bh, byte[y]
    mov bl, -1
    cmp bh, bl
    je dl2dr

    jmp display

dl2ul:
    mov byte[x], 23
    mov byte[rdul], _UL
    jmp display

dl2dr:
    mov byte[y], 1
    mov byte[rdul], _DR
    jmp display
U_R:
    dec byte[x]
    inc byte[y]

    mov bh, byte[x]
    mov bl, -1
    cmp bh, bl
    je ur2dr

    mov bh, byte[y]
    mov bl, 80
    cmp bh, bl
    je ur2ul

    jmp display

ur2dr:
    mov byte[x], 1
    mov byte[rdul], _DR
    jmp display

ur2ul:
    mov byte[y], 78
    mov byte[rdul], _UL
    jmp display

U_L:
    dec byte[x]
    dec byte[y]

    mov bh, byte[x]
    mov bl, -1
    cmp bh, bl
    je ul2dl

    mov bh, byte[y]
    mov bl, -1
    cmp bh, bl
    je ul2ur

    jmp display

ul2dl:
    mov byte[x], 1
    mov byte[rdul], _DL
    jmp display

ul2ur:
    mov byte[y], 1
    mov byte[rdul], _UR
    jmp display

display:
    cmp byte[num],12
    je swap
next:
    cmp byte[si],0
    je swap
    xor ax, ax
    mov ax, word[x]
    mov bx, 80
    mul bx
    add ax, word[y]
    mov bx, 2
    mul bx
    mov bx, ax
    mov ah, byte[color];
    mov al, byte[si]
    mov [es:bx], ax
    inc si

change_color:
    inc byte[color]
    cmp byte[color], 0x0f
    jnz Loop
    mov byte[color], 0x02
    jmp Loop
swap:
    sub si, 12
    jmp next
end:
    jmp $

count dw delay
dcount dw ddelay
rdul db _DR
color db 0x02
x dw 0
y dw 0
name db '22336173_LHJ',0
num db 0

times 510 - ($ - $$) db 0
db 0x55, 0xaa
