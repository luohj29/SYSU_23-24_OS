org 0x7e00
%include "boot.inc"
[bits 16]
mov ax, 0xb800 ;�Դ�ε�ַ
mov gs, ax
mov ah, 0x03 ;��ɫ
mov ecx, bootloader_tag_end - bootloader_tag
xor ebx, ebx
mov esi, bootloader_tag
output_bootloader_tag:
    mov al, [esi]
    mov word[gs:bx], ax
    inc esi
    add ebx,2
    loop output_bootloader_tag

;��������
mov dword [GDT_START_ADDRESS+0x00],0x00
mov dword [GDT_START_ADDRESS+0x04],0x00  

;����������������һ�����ݶΣ���Ӧ0~4GB�����Ե�ַ�ռ�
mov dword [GDT_START_ADDRESS+0x08],0x0000ffff    ; ����ַΪ0���ν���Ϊ0xFFFFF
mov dword [GDT_START_ADDRESS+0x0c],0x00cf9200    ; ����Ϊ4KB���洢���������� 

;��������ģʽ�µĶ�ջ��������      
mov dword [GDT_START_ADDRESS+0x10],0x00000000    ; ����ַΪ0x00000000������0x0 
mov dword [GDT_START_ADDRESS+0x14],0x00409600    ; ����Ϊ1���ֽ�

;��������ģʽ�µ��Դ�������   
mov dword [GDT_START_ADDRESS+0x18],0x80007fff    ; ����ַΪ0x000B8000������0x07FFF 
mov dword [GDT_START_ADDRESS+0x1c],0x0040920b    ; ����Ϊ�ֽ�

;��������ģʽ��ƽ̹ģʽ�����������
mov dword [GDT_START_ADDRESS+0x20],0x0000ffff    ; ����ַΪ0���ν���Ϊ0xFFFFF
mov dword [GDT_START_ADDRESS+0x24],0x00cf9800    ; ����Ϊ4kb������������� 


;��ʼ����������Ĵ���GDTR
mov word [pgdt], 39      ;��������Ľ���   
lgdt [pgdt]

; _____________Selector_____________
;ƽ̹ģʽ���ݶ�ѡ����
DATA_SELECTOR equ 0x8
;ƽ̹ģʽջ��ѡ����
STACK_SELECTOR equ 0x10
;ƽ̹ģʽ��Ƶ��ѡ����
VIDEO_SELECTOR equ 0x18
VIDEO_NUM equ 0x18
;ƽ̹ģʽ�����ѡ����
CODE_SELECTOR equ 0x20

in al,0x92                         ;����оƬ�ڵĶ˿� 
or al,0000_0010B
out 0x92,al                        ;��A20

cli                                ;�жϻ�����δ����
mov eax,cr0
or eax,1
mov cr0,eax                        ;����PEλ

jmp dword CODE_SELECTOR:protect_mode_begin

;16λ��������ѡ���ӣ�32λƫ��
;����ˮ�߲����л�������
[bits 32]           
protect_mode_begin:                              

mov eax, DATA_SELECTOR                     ;�������ݶ�(0..4GB)ѡ����
mov ds, eax
mov es, eax
mov eax, STACK_SELECTOR
mov ss, eax
mov eax, VIDEO_SELECTOR
mov gs, eax

mov ecx, protect_mode_tag_end - protect_mode_tag
mov ebx, 80 * 2
mov esi, protect_mode_tag
mov ah, 0x3
output_protect_mode_tag:
    mov al, [esi]
    mov word[gs:ebx], ax
    add ebx, 2
    inc esi
    loop output_protect_mode_tag


mov eax, KERNEL_START_SECTOR
mov ebx, KERNEL_START_ADDRESS
mov ecx, KERNEL_SECTOR_COUNT

load_kernel: 
    push eax
    push ebx
    call asm_read_hard_disk  ; ��ȡӲ��
    add esp, 8
    inc eax
    add ebx, 512
    loop load_kernel

jmp dword CODE_SELECTOR:KERNEL_START_ADDRESS       ; ��ת��kernel

jmp $ ; ��ѭ��

asm_read_hard_disk:                           
    push ebp
    mov ebp, esp

    push eax
    push ebx
    push ecx
    push edx

    mov eax, [ebp + 4 * 3] ; �߼�������16λ

    mov edx, 0x1f3
    out dx, al    ; LBA��ַ7~0

    inc edx        ; 0x1f4
    mov al, ah
    out dx, al    ; LBA��ַ15~8

    xor eax, eax
    inc edx        ; 0x1f5
    out dx, al    ; LBA��ַ23~16 = 0

    inc edx        ; 0x1f6
    mov al, ah
    and al, 0x0f
    or al, 0xe0   ; LBA��ַ27~24 = 0
    out dx, al

    mov edx, 0x1f2
    mov al, 1
    out dx, al   ; ��ȡ1������

    mov edx, 0x1f7    ; 0x1f7
    mov al, 0x20     ;������
    out dx,al

    ; �ȴ�������������
  .waits:
    in al, dx        ; dx = 0x1f7
    and al,0x88
    cmp al,0x08
    jnz .waits                         
    

    ; ��ȡ512�ֽڵ���ַds:bx
    mov ebx, [ebp + 4 * 2]
    mov ecx, 256   ; ÿ�ζ�ȡһ���֣�2���ֽڣ���˶�ȡ256�μ���          
    mov edx, 0x1f0
  .readw:
    in ax, dx
    mov [ebx], eax
    add ebx, 2
    loop .readw
      
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp

    ret

pgdt dw 0
    dd GDT_START_ADDRESS

bootloader_tag db 'bootloader'
bootloader_tag_end:

protect_mode_tag db 'enter protect mode'
protect_mode_tag_end: