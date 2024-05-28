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
mov ax, 1               ; �߼������ŵ�0~15λ
mov cx, 0                ; �߼������ŵ�16~31λ
mov bx, 0x7e00           ; bootloader�ļ��ص�ַ
load_bootloader:
    call asm_read_hard_disk  ; ��ȡӲ��
    inc ax
    cmp ax, 5
    jle load_bootloader
jmp 0x0000:0x7e00        ; ��ת��bootloader

jmp $ ; ��ѭ��

asm_read_hard_disk:                           
; ��Ӳ�̶�ȡһ���߼�����

; �����б�
; ax=�߼�������0~15λ
; cx=�߼�������16~28λ
; ds:bx=��ȡ�������ݷ����ַ

; ����ֵ
; bx=bx+512

    mov dx, 0x1f3
    out dx, al    ; LBA��ַ7~0

    inc dx        ; 0x1f4
    mov al, ah
    out dx, al    ; LBA��ַ15~8

    mov ax, cx

    inc dx        ; 0x1f5
    out dx, al    ; LBA��ַ23~16

    inc dx        ; 0x1f6
    mov al, ah
    and al, 0x0f
    or al, 0xe0   ; LBA��ַ27~24
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
    mov cx, 256   ; ÿ�ζ�ȡһ���֣�2���ֽڣ���˶�ȡ256�μ���          
    mov dx, 0x1f0
  .readw:
    in ax, dx
    mov [bx], ax
    add bx, 2
    loop .readw
      
    ret

times 510 - ($ - $$) db 0
db 0x55, 0xaa