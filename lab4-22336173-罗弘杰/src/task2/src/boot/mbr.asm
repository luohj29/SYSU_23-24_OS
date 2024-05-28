%include "boot.inc"
org 0x7c00
[bits 16]
xor ax, ax ; eax = 0
; ��ʼ���μĴ���, �ε�ַȫ����Ϊ0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; ��ʼ��ջָ��
mov sp, 0x7c00    

mov ax, LOADER_START_SECTOR
mov cx, LOADER_SECTOR_COUNT
mov bx, LOADER_START_ADDRESS   

load_bootloader: 
    push ax
    push bx
    call asm_read_hard_disk  ; ��ȡӲ��
    add sp, 4
    inc ax
    add bx, 512
    loop load_bootloader

    jmp 0x0000:0x7e00        ; ��ת��bootloader

jmp $ ; ��ѭ��

; asm_read_hard_disk(memory,block)
; �����߼�������Ϊblock���������ڴ��ַmemory

asm_read_hard_disk:                           
    push bp
    mov bp, sp

    push ax
    push bx
    push cx
    push dx

    mov ax, [bp + 2 * 3] ; �߼�������16λ

    mov dx, 0x1f3
    out dx, al    ; LBA��ַ7~0

    inc dx        ; 0x1f4
    mov al, ah
    out dx, al    ; LBA��ַ15~8

    xor ax, ax
    inc dx        ; 0x1f5
    out dx, al    ; LBA��ַ23~16 = 0

    inc dx        ; 0x1f6
    mov al, ah
    and al, 0x0f
    or al, 0xe0   ; LBA��ַ27~24 = 0
    out dx, al

    mov dx, 0x1f2
    mov al, 1
    out dx, al   ; ��ȡ1������

    mov dx, 0x1f7    ; 0x1f7
    mov al, 0x20     ;������
    out dx,al

    ; �ȴ�������������
  .waits:
    in al, dx        ; dx = 0x1f7
    and al,0x88
    cmp al,0x08
    jnz .waits                         
    

    ; ��ȡ512�ֽڵ���ַds:bx
    mov bx, [bp + 2 * 2]
    mov cx, 256   ; ÿ�ζ�ȡһ���֣�2���ֽڣ���˶�ȡ256�μ���          
    mov dx, 0x1f0
  .readw:
    in ax, dx
    mov [bx], ax
    add bx, 2
    loop .readw
      
    pop dx
    pop cx
    pop bx
    pop ax
    pop bp

    ret

times 510 - ($ - $$) db 0
db 0x55, 0xaa