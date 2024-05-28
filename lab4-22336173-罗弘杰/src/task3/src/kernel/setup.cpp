#include "asm_utils.h"
#include "interrupt.h"

// �жϹ�����
InterruptManager interruptManager;

extern "C" void setup_kernel()
{   
    asm_hello_world();
    // �жϴ�����
    interruptManager.initialize();

    // ���Դ�����0����
    int a = 1 / 0;

    // ��ѭ��
    asm_halt();
}