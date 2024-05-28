#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"

// 灞忓箷IO澶勭悊鍣?
STDIO stdio;
// 涓柇绠＄悊鍣?
InterruptManager interruptManager;
// 绋嬪簭绠＄悊鍣?
ProgramManager programManager;

void fourth_thread(void *arg) {
    printf("pid %d name \"%s\": Hello World!\n", programManager.running->pid, programManager.running->name);
    asm_halt();
}

void third_thread(void *arg) {
    printf("pid %d name \"%s\": Hello World!\n", programManager.running->pid, programManager.running->name);
    asm_halt();
}

void second_thread(void *arg) {
    printf("pid %d name \"%s\": Hello World!\n", programManager.running->pid, programManager.running->name);
    asm_halt();
}

void first_thread(void *arg)
{               // 输出信息，表明线程已经开始运行
    printf("pid %d name \"%s\": Hello World!\n", programManager.running->pid, programManager.running->name);
    asm_halt();
}

extern "C" void setup_kernel()
{

    // 涓柇绠＄悊鍣?
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt(); // 开启时钟中断
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler); // 设置时钟中断处理函数

    // 杈撳嚭绠＄悊鍣?
    stdio.initialize();

    // 杩涚▼/绾跨▼绠＄悊鍣?
    programManager.initialize();

    // 鍒涘缓绗竴涓嚎绋?
    int pid0 = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    int pid1 = programManager.executeThread(second_thread, nullptr, "second thread", 1);
    int pid2 = programManager.executeThread(third_thread, nullptr, "third thread", 1);
	int pid3 = programManager.executeThread(fourth_thread, nullptr, "fourth thread", 1);
    if (pid0 == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    //手动切换到第一个线程
    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread); //从内核态切换到用户态

    asm_halt(); //
}
