#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
// ����������2������4�Ĵ��룬ʹ�����ȼ������㷨�����ȼ�Խ�ߵ��߳�Խ��ִ��
// 屏幕IO处理�?
STDIO stdio;
// �?�?管理�?
InterruptManager interruptManager;
// 程序管理�?
ProgramManager programManager;

void fourth_thread(void *arg) {
    printf("pid %d name \"%s  %d\": Hello World!\n", programManager.running->pid, programManager.running->name, programManager.running->priority);
}

void third_thread(void *arg) {
    printf("pid %d name \"%s  %d\": Hello World!\n", programManager.running->pid, programManager.running->name, programManager.running->priority);
}

void second_thread(void *arg) {
    printf("pid %d name \"%s  %d\": Hello World!\n", programManager.running->pid, programManager.running->name, programManager.running->priority);
}

void first_thread(void *arg)
{               // �����Ϣ�������߳��Ѿ���ʼ����
    printf("pid %d name \"%s  %d\": Hello World!\n", programManager.running->pid, programManager.running->name, programManager.running->priority);
}

extern "C" void setup_kernel()
{

    // �?�?管理�?
    interruptManager.initialize();
    interruptManager.disableTimeInterrupt(); // ����ʱ���ж�
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler); // ����ʱ���жϴ�����

    // 输出管理�?
    stdio.initialize();

    // 进程/线程管理�?
    programManager.initialize();

    // 创建�?一�?线�??
    int pid0 = programManager.executeThread(first_thread, nullptr, "first thread", 1); //���ȼ�Ϊ1
    int pid1 = programManager.executeThread(second_thread, nullptr, "second thread", 3); //���ȼ�Ϊ3
    int pid2 = programManager.executeThread(third_thread, nullptr, "third thread", 2); //���ȼ�Ϊ2
	int pid3 = programManager.executeThread(fourth_thread, nullptr, "fourth thread", 4); //���ȼ�Ϊ4
    if (pid0 == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    //�ֶ��л�����һ���߳�
    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread); //���ں�̬�л����û�̬

    asm_halt(); //
}
