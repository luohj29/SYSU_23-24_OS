#include "stdio.h"
#include "os_type.h"
#include "os_modules.h"
#include "asm_utils.h"
#include "stdlib.h"
#include "stdarg.h"


STDIO::STDIO()
{
    initialize();
}

void STDIO::initialize()
{
    screen = (uint8 *)0xb8000;
}

void STDIO::print(uint x, uint y, uint8 c, uint8 color)
{

    if (x >= 25 || y >= 80)
    {
        return;
    }

    uint pos = x * 80 + y;
    screen[2 * pos] = c;
    screen[2 * pos + 1] = color;
}

void STDIO::print(uint8 c, uint8 color)
{
    uint cursor = getCursor();
    screen[2 * cursor] = c;
    screen[2 * cursor + 1] = color;
    cursor++;
    if (cursor == 25 * 80)
    {
        rollUp();
        cursor = 24 * 80;
    }
    moveCursor(cursor);
}

void STDIO::print(uint8 c)
{
    print(c, 0x07); // 默认颜色是黑底白字
}

void STDIO::moveCursor(uint position)
{
    if (position >= 80 * 25)
    {
        return;
    }

    uint8 temp;

    // 处理高8位
    temp = (position >> 8) & 0xff;
    asm_out_port(0x3d4, 0x0e);
    asm_out_port(0x3d5, temp);

    // 处理低8位
    temp = position & 0xff;
    asm_out_port(0x3d4, 0x0f);
    asm_out_port(0x3d5, temp);
}

uint STDIO::getCursor()
{
    uint pos;
    uint8 temp;

    pos = 0;
    temp = 0;
    // 处理高8位
    asm_out_port(0x3d4, 0x0e);
    asm_in_port(0x3d5, &temp);
    pos = ((uint)temp) << 8;

    // 处理低8位
    asm_out_port(0x3d4, 0x0f);
    asm_in_port(0x3d5, &temp);
    pos = pos | ((uint)temp);

    return pos;
}

void STDIO::moveCursor(uint x, uint y)
{
    if (x >= 25 || y >= 80)
    {
        return;
    }

    moveCursor(x * 80 + y);
}

void STDIO::rollUp()
{
    uint length;
    length = 25 * 80;
    for (uint i = 80; i < length; ++i)
    {
        screen[2 * (i - 80)] = screen[2 * i];
        screen[2 * (i - 80) + 1] = screen[2 * i + 1];
    }

    for (uint i = 24 * 80; i < length; ++i)
    {
        screen[2 * i] = ' ';
        screen[2 * i + 1] = 0x07;
    }
}

// 实现print(const char *const str)函数
int STDIO::print(const char *const str)
{
    int i = 0;

    for (i = 0; str[i]; ++i)
    {
        switch (str[i])
        {
        case '\n':
            uint row;
            row = getCursor() / 80;
            if (row == 24)
            {
                rollUp();
            }
            else
            {
                ++row;
            }
            moveCursor(row * 80);
            break;

        default:
            print(str[i]);
            break;
        }
    }

    return i;
}

/*
实现printf_add_to_buffer函数,
*/
int printf_add_to_buffer(char *buffer, char c, int &idx, const int BUF_LEN)  //idx是引用传递
{
    int counter = 0;

    buffer[idx] = c;
    ++idx;

    if (idx == BUF_LEN)
    {
        buffer[idx] = '\0';
        counter = stdio.print(buffer);
        idx = 0;
    }

    return counter;
}

/*
实现c语言的printf函数
首先我们定义一个大小为BUF_LEN的缓冲区buffer，buffer多出来的1个字符是用来放置\0的。
由于我们后面会将一个整数转化为字符串表示，number使用来存放转换后的数字字符串。
由于保护模式是运行在32位环境下的，最大的数字字符串也不会超过32位，
因此number分配33个字节也就足够了。

接着我们开始对fmt进行逐字符解析，对于每一个字符fmt[i]，如果fmt[i]不是%，则说明是普通字符，直接放到缓冲区即可。
注意，将fmt[i]放到缓冲区后可能会使缓冲区变满，此时如果缓冲区满，则将缓冲区输出并清空，
我们不妨上述过程写成一个函数来实现，如下所示。
*/

int printf(const char *const fmt, ...) // fmt是格式化字符串
{
    const int BUF_LEN = 32;

    char buffer[BUF_LEN + 1];
    char number[33];


    va_list ap; // 定义一个指向参数的指针

    va_start(ap, fmt); // 将ap指向fmt后面的第一个参数
    int idx = 0; // 初始化缓冲区索引， idx表示当前缓冲区中的字符个数
    int counter = 0; // 初始化计数器， counter表示已经输出的字符个数

    for (int i = 0; fmt[i]; ++i)
    {
        if (fmt[i] != '%') // 如果fmt[i]不是%，则说明是普通字符，直接放到缓冲区即可
        {
            counter += printf_add_to_buffer(buffer, fmt[i], idx, BUF_LEN); // 将fmt[i]放到缓冲区
        }
        else // 如果fmt[i]是%，则说明后面有参数需要处理
        {
            i++;
            if (fmt[i] == '\0') // 如果fmt[i]是字符串结束符，则说明fmt不合法，直接退出
            {
                break;
            }

            switch (fmt[i])
            {
            case '%':   // 如果fmt[i]是%，则说明需要打印%，直接放到缓冲区即可
                counter += printf_add_to_buffer(buffer, fmt[i], idx, BUF_LEN);
                break;

            case 'v':  //输出作者水印 “22336173——LHJ”
                buffer[idx] = '\0'; //最后一个字符是\0
                idx = 0;
                counter += stdio.print(buffer); // 将缓冲区中的内容输出             
                counter += printf_add_to_buffer(buffer, '2', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, '2', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, '3', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, '3', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, '6', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, '1', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, '7', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, '3', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, '——', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, 'L', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, 'H', idx, BUF_LEN);
                counter += printf_add_to_buffer(buffer, 'J', idx, BUF_LEN);
                break;

            case 'c': // 如果fmt[i]是c，则说明需要打印一个字符，直接放到缓冲区即可
                counter += printf_add_to_buffer(buffer, va_arg(ap, int), idx, BUF_LEN); // 将字符放到缓冲区
                break;

            case 's': // 如果fmt[i]是s，则说明需要打印一个字符串，直接放到缓冲区即可
                buffer[idx] = '\0'; //最后一个字符是\0
                idx = 0;
                counter += stdio.print(buffer); // 将缓冲区中的内容输出
                counter += stdio.print(va_arg(ap, const char *)); // 将字符串放到缓冲区
                break;

            case 'd': //
            case 'x':
                int temp = va_arg(ap, int);

                if (temp < 0 && fmt[i] == 'd')
                {
                    counter += printf_add_to_buffer(buffer, '-', idx, BUF_LEN);
                    temp = -temp;
                }

                itos(number, temp, (fmt[i] == 'd' ? 10 : 16));

                for (int j = 0; number[j]; ++j)
                {
                    counter += printf_add_to_buffer(buffer, number[j], idx, BUF_LEN);
                }
                break;

            }
        }
    }

    buffer[idx] = '\0';
    counter += stdio.print(buffer); // 将缓冲区中的内容输出

    return counter;
}
