
org 0x7c00
[bits 16]
xor ax, ax ; eax = 0
; 初始化段寄存器, 段地址全部设为0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; 初始化栈指针
mov sp, 0x7c00
mov ax, 1                ; 逻辑扇区号第0~15位
mov cx, 0                ; 逻辑扇区号第16~31位
mov bx, 0x7e00           ; bootloader的加载地址

mov ax, 2                ; 物理扇区第2位
load_bootloader:
    call asm_read_hard_disk  ; 读取硬盘
    inc ax
    cmp ax, 5
    jle load_bootloader
jmp 0x0000:0x7e00        ; 跳转到bootloader

jmp $ ; 死循环

asm_read_hard_disk:                           
; 从硬盘读取一个逻辑扇区

; 参数列表
; ax=起始扇区号

; 返回值
; bx=bx+512
; ax=ax+1
    mov dl, 80h
    mov dh, 0; 磁头号
    mov ch, 0; 柱面号
    mov cl, al; 扇区号


    mov ah, 2; 功能号
    mov al, 1; 扇区数
    int 13h; 调用int 13h中断

    add bx, 512; bx=bx+512, 读取下一个扇区
    ret

times 510 - ($ - $$) db 0
db 0x55, 0xaa