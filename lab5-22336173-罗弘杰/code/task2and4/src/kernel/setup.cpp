#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
// 以下是任务2和任务4的代码，使用优先级调度算法，优先级越高的线程越先执行
// 灞忓箷IO澶勭悊鍣?
STDIO stdio;
// 涓?鏂?绠＄悊鍣?
InterruptManager interruptManager;
// 绋嬪簭绠＄悊鍣?
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
{               // 输出信息，表明线程已经开始运行
    printf("pid %d name \"%s  %d\": Hello World!\n", programManager.running->pid, programManager.running->name, programManager.running->priority);
}

extern "C" void setup_kernel()
{

    // 涓?鏂?绠＄悊鍣?
    interruptManager.initialize();
    interruptManager.disableTimeInterrupt(); // 开启时钟中断
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler); // 设置时钟中断处理函数

    // 杈撳嚭绠＄悊鍣?
    stdio.initialize();

    // 杩涚▼/绾跨▼绠＄悊鍣?
    programManager.initialize();

    // 鍒涘缓绗?涓�涓?绾跨??
    int pid0 = programManager.executeThread(first_thread, nullptr, "first thread", 1); //优先级为1
    int pid1 = programManager.executeThread(second_thread, nullptr, "second thread", 3); //优先级为3
    int pid2 = programManager.executeThread(third_thread, nullptr, "third thread", 2); //优先级为2
	int pid3 = programManager.executeThread(fourth_thread, nullptr, "fourth thread", 4); //优先级为4
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
