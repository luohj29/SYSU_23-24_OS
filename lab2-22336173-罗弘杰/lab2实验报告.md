

![image-20240315232430405](C:\Users\rogers\AppData\Roaming\Typora\typora-user-images\image-20240315232430405.png)

![中大校名校徽-Green](C:\Program Files\Typora\media\image1.png)

# 			**本科生实验报告**

实验课程: 操作系统原理实验	

任课教师: 刘宁

实验题目:实模式和保护模式下的OS启动

专业名称: 信息与计算科学

学生姓名:罗弘杰

学生学号: 22336173

实验地点: 实验中心D503

实验时间: 2024/3/15

## 				**Section 1 实验概述**

在第二章中，同学们将会学习到x86汇编、计算机的启动过程、IA-32处理器架构和字符显存原理。根据所学的知识，同学们能自己编写程序，然后让计算机在启动后加载运行，以此增进同学们对计算机启动过程的理解，为后面编写操作系统加载程序奠定基础。同时，同学们将学习如何使用gdb来调试程序的基本方法。

##                   **Section 2 预备知识与实验环境**

该节总结实验需要用到的基本知识，以及主机型号、代码编译工具、重要三方库的版本号信息等。

- 预备知识：x86汇编语言程序设计、Linux系统命令行工具，qemu虚拟机模拟，gdb调试工具

- IA-32处理器将显示矩阵映射到内存地址0xB8000~0xBFFFF处，这段地址称为显存地址。在文本模式下，控制器的最小可控制单位为字符。每一个显示字符自上而下，从左到右依次使用0xB8000~0xBFFFF中的两个字节表示。在这两个字节中，低字节表示显示的字符，高字节表示字符的颜色属性，如下所示。

- ![](C:\Users\rogers\AppData\Roaming\Typora\typora-user-images\image-20240315235718202.png)

- 实验环境：

  - 虚拟机版本/处理器型号：ubuntu-18.0.4 , 阿里云服务器  通用cpu

  - 代码编辑环境： vim

  - 代码编译工具： gcc，nasm

  - 重要三方库信息：无

##                             **Section 3 实验任务**

该节描述需要完成的几个实验任务，即重述实验题目的总要求，建议使用项目编号分点阐述。详细要求可在下一节【实验步骤与实验结果】中列出。

### 实验任务1：

​	学习x86汇编基础，理解实模式下计算机启动的过程，复现 “操作系统的启动Hello World--编写MBR”部分的实验。

### 实验任务2： 

​	探索实模式中断，利用中断实现光标移动和在光标处打印字符等等

### 实验任务3：

​	汇编代码实现分支逻辑，循环逻辑，以及函数的实现

### 实验任务4：

​	实现一个字符弹射程序

## 			**Section 4 实验步骤与实验结果**

​	该节描述每个实验任务的具体的完成过程，包括思路分析、代码实现与执行、结果展示三个部分，实验任务之间的划分应当清晰明了，实验思路分析做到有逻辑、有条理。

### 	         ------------------------- 实验任务1-------------------------   

#### 任务要求：

​	编写汇编代码，编译后加入MBR中，启动qemu,读取MBR,显示“hello world”

#### 思路分析：

​	参考实验资料给出的代码，将helloworld字符加载到0xB8000~0xBFFFF的显存内，注意每个字符由两个字节表示，低字节表示字符的内容，高字节表示颜色（前四位表示背景色，后四位表示前景色）。结尾加入一段死循环，让字符串恒定显示。 注意物理地址的计算公式：物理地址=gs<<4+2∗1=0xB800<<4+2∗1=0xB8002；在开始的时候要给gs赋值到0x800再左移四位就到达显存位置。

```asm
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
mov ax, 0xb800;  初始化显存的段寄存器
mov gs, ax


mov ah, 0x01 ; 蓝色  ；规定颜色
mov al, 'H'
mov [gs:2 * 0], ax

mov al, 'e'
mov [gs:2 * 1], ax

mov al, 'l'
mov [gs:2 * 2], ax

mov al, 'l'
mov [gs:2 * 3], ax

mov al, 'o'
mov [gs:2 * 4], ax

mov al, ' '
mov [gs:2 * 5], ax

mov al, 'W'
mov [gs:2 * 6], ax

mov al, 'o'
mov [gs:2 * 7], ax

mov al, 'r'
mov [gs:2 * 8], ax

mov al, 'l'
mov [gs:2 * 9], ax

mov al, 'd'
mov [gs:2 * 10], ax

jmp $ ; 死循环

times 510 - ($ - $$) db 0
db 0x55, 0xaa
```

#### 实验结果：![屏幕截图 2024-03-09 175531](C:\Users\rogers\Pictures\Screenshots\屏幕截图 2024-03-09 175531.png)



###                  ------------------------- **实验任务2**-------------------------

#### 任务要求：

![image-20240315233114872](C:\Users\rogers\AppData\Roaming\Typora\typora-user-images\image-20240315233114872.png)

#### 思路分析：

​	根据实验资料给出的关于实模式下中断的功能号的功能，包括了光标位置的获取，光标的移动和在光标处打印字符。具体地：应该先设置或者获取光标的初始位置，然后根据打印的字符数来设计光标的移动，注意显示屏上横纵坐标上限分别为[0,24],[0,79];  在必要的时候需要安排光标换行。



#### 实验步骤：

##### （1）**利用中断实现光标的位置获取和光标的移动**

###### 代码展示：

```assembly
org 0x7c00
[bits 16]
; get cursor position
mov ah, 0x03; get cursor position
mov bh, 0; page number
int 10h

; set cursor position
mov ah, 0x02; set cursor position
mov bh, 0; page number
inc dh; increment row
inc dl; increment column
int 10h

jmp $

times 510-($-$$) db 0
db 0x55, 0xaa

```

第1、2行的`org 0x7c00`和`[bits 16]`是汇编伪指令，不是实际的指令。`org 0x7c00`是告诉编译器代码中的代码标号和数据标号从`0x7c00`开始。也就是说，这些标号的地址会在编译时加上`0x7c00`

1.获取光标的功能号为03h，将获取的行号，列号写入dl,dh寄存器；

2.移动字符的功能号为02h,  在这里我将行号和列号分别加一，即向右下角移动一次；

###### 成果展示：

##### （2）请修改Hello World的代码，**使用实模式下的中断来输出你的学号**。说说你是怎么做的，并将结果截图。

```assembly
my_id db "22336173_LHJ"; 
org 0x7c00
[bits 16]
; print"22336173" on the screen

set_cursor_first:
    mov bx, 0
    mov dh, 5; row5
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
    call cursor_inc  ; cursor movement
    pop cx
    add si, 1; next char
    loop print_loop

    jmp $
    
cursor_inc:
    inc dl
    cmp dl, 80
    jne add_cursor
    mov dl, 0; 换行
    inc dh
add_cursor:
    mov ah, 2; set cursor +1
    int 10h
    ret

times 510-($-$$) db 0
db 0x55, 0xaa

```

###### 代码分析：

​	使用循环输出的思路，首先将我的学号放在字符串数据段，然后将地址放入si寄存器，循环次数放入ecx寄存器，每次输出完以后，ecx-1, si加一以打印下一个字符；注意光标移动需要添加换行条件机制。

###### 成果展示：

![屏幕截图 2024-03-14 225034](C:\Users\rogers\Pictures\Screenshots\屏幕截图 2024-03-14 225034.png)

##### （3）在1和2的知识的基础上，探索实模式的键盘中断，**利用键盘中断实现键盘输入并回显**，可以参考[https://blog.csdn.net/deniece1/article/details/103447413](https://gitee.com/link?target=https%3A%2F%2Fblog.csdn.net%2Fdeniece1%2Farticle%2Fdetails%2F103447413)。关于键盘扫描码，可以参考[http://blog.sina.com.cn/s/blog_1511e79950102x2b0.html](https://gitee.com/link?target=http%3A%2F%2Fblog.sina.com.cn%2Fs%2Fblog_1511e79950102x2b0.html)。说说你是怎么做的，并将结果截图。

##### 代码展示：

```assembly
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

```

##### 代码分析：

​	考虑使用中断号16h，功能号00h, 资料如下：![image-20240315235209719](C:\Users\rogers\AppData\Roaming\Typora\typora-user-images\image-20240315235209719.png)

​	每次都测试键盘有无输入，通过al寄存器与0相或来判断，无输入则循环等待输入，否则就进入输出模块**，依然要考虑换行机制，输出完以后，返回等待输出。**

##### 成果展示：

![屏幕截图 2024-03-15 141448](C:\Users\rogers\Pictures\Screenshots\屏幕截图 2024-03-15 141448.png)

### 		------------------------- 实验任务3-------------------------

#### 任务要求：![image-20240319130201653](C:\Users\rogers\AppData\Roaming\Typora\typora-user-images\image-20240319130201653.png)

#### 思路分析：

本题要求我们熟悉x86汇编的条件判断和循环和函数的结构。条件： cmp, je,jne,jz,jnz等等；循环： 使用cx寄存器存储循环次数，loop指令更新循环次数； 函数： call ,ret 注意需要使用栈寄存器存储原函数环境，可以使用pushad,popad指令来快速保存和回复环境。

#### 代码分析：

```assembly
[bits 32]
%include "head.include"

    mov eax, [a1]
    mov ebx, [a2]
your_if:  ;条件循环
    cmp eax, 12
    jl lt12 ; if eax < 12 then jump to lt12
    cmp eax, 24
    jl lt24 ; if eax < 24 then jump to lt24
    shl eax, 4; a1*=16
    mov [if_flag], eax; change if_flag to a1
    jmp your_while

lt24:
    mov ecx, eax ; ecx = eax
    sub ecx, 24 ; ecx = ecx - 24
    neg ecx ; ecx = -ecx
    imul ecx, eax; ecx = ecx * eax
    mov [if_flag], ecx
    jmp your_while

lt12:
    sar eax, 1 ; a1/=2
    inc eax ; a1++
    mov [if_flag], eax
    jmp your_while

your_while:   ;循环
    cmp dword[a2], 12
    jl lt_12; if ebx < 12 then jump to lt12
    call my_random
    mov ebx, [a2]
    mov dword[ecx + ebx - 12], eax
    dec dword[a2]
    jmp your_while

lt_12:
%include "end.include"

your_function:  ;函数
    pushad
    mov eax, 0
loop:
    mov ecx, [your_string]
    cmp byte[ecx+eax], 0
    je loopend
    pushad
    mov ebx, dword[ecx+eax]
    push ebx
    call print_a_char
    pop ebx
    popad
    add eax, 1;
    jmp loop;

loopend:
    popad
    ret
```

#### 结果展示：

![屏幕截图 2024-03-15 194953](C:\Users\rogers\Pictures\Screenshots\屏幕截图 2024-03-15 194953.png)

​	通过了测试。

###                        ------------------------- *实验任务*4 -------------------------

#### 任务要求：

​	字符弹射程序。请编写一个字符弹射程序，其从点(2,0)处开始向右下角45度开始射出，遇到边界反弹，反弹后按45度角射出，方向视反弹位置而定。同时，你可以加入一些其他效果，如变色，双向射出等。注意，你的程序应该不超过510字节，否则无法放入MBR中被加载执行。

​	![新建虚拟机14](https://gitee.com/nelsoncheung/sysu-2023-spring-operating-system/raw/main/lab2/gallery/bonus-1.PNG)

#### 思路分析：

 	依然使用存储显存的办法，设置初始坐标为（2，0）。然后设置4个移动函数，分别对应左上，左下，右下，右上；在每个函数中都设置碰壁判断函数（x:-1和25，y:-1和80），一旦碰撞就触发反弹函数。 每次移动以后加入字符添加函数，加入颜色变化函数，每次存储一个字符，循环存储的是字符串“22336173_LHJ”



#### 代码分析：

```assembly
org 0x7c00
; 初始化规定变量
_DR equ 1
_UR equ 2
_UL equ 3
_DL equ 4    ; 这四个分别是四个方向的代号
delay equ 2000   
ddelay equ 100   ;循环延迟的量  总共是2000*100个循环以延迟字符输出的间隔


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
    je D_R                ;判断当前移动方向，下同

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

D_R:        ;进入判断碰壁函数，若碰壁则相应的改变方向，并改变当前的移动方向
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

    jmp display     ;结束后进入输出阶段

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
    cmp byte[num],12  ;12是我的字符串的个数
    je swap
next:
    xor ax, ax
    mov ax, word[x] ; 获取横纵坐标，根据显存计算公式：es<<4+2*（x*80+y）来获得当前存储的位置
    mov bx, 80
    mul bx
    add ax, word[y]
    mov bx, 2
    mul bx
    mov bx, ax
    mov ah, byte[color];
    mov al, byte[si]   ;si存储的是当前应该输出的字符的位置
    mov [es:bx], ax
    inc si  ;准备输出下一个字符

change_color:   ;颜色变化，恒定背景为黑色，前景不断变化颜色
    inc byte[color]
    cmp byte[color], 0x0f 
    jnz Loop
    mov byte[color], 0x02
    jmp Loop
swap:     ;若输出到结尾则变回字符串的头部
    sub si, 12
    jmp next
end:
    jmp $

count dw delay   ; count to loop for latency
dcount dw ddelay
rdul db _DR   ; current movement
color db 0x02   ;initial color
x dw 0   ; position code
y dw 0
name db '22336173_LHJ',0    ;my id
num db 0   ; time that haved printed

times 510 - ($ - $$) db 0
db 0x55, 0xaa

```



#### 成果展示：

![屏幕截图 2024-03-18 233543](C:\Users\rogers\Pictures\Screenshots\屏幕截图 2024-03-18 233543.png)

​	![屏幕截图 2024-03-18 233606](C:\Users\rogers\Pictures\Screenshots\屏幕截图 2024-03-18 233606.png)

实验结果展示：通过执行前述代码，可得下图结果：

![](media/image5.jpeg)





## **Section 5 实验总结与心得体会**

棘手的问题：汇编代码掌握不完全，对于寄存器偏移寻址掌握不熟。
