#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

Semaphore SMOKER_MATCH;
Semaphore SMOKER_PAPER;
Semaphore SMOKER_TOBACCO;
Semaphore AGENT;  //供应商的信号量，会被初始化为1，表示可以开始放物资
void producer(void *arg)
{   
    AGENT.initialize(1); // 1 means the agent is available
    int rand = 0;
    while(1){
        //printf("producer thread running!\n");
        AGENT.P(); 
        int rand = (rand +1)% 3;
        printf("offer material %d\n", rand+1);
        if (rand == 0){
            SMOKER_MATCH.V();//smoker1
        }else if (rand == 1){
            SMOKER_PAPER.V();//smoker2
        }else{
            SMOKER_TOBACCO.V();//smoker3
        }
    }
}
    

void smoker1(void *arg)
{
    while(1){
        SMOKER_MATCH.P(); //smoker1
        printf("smoker1: I am smoking!\n");
        AGENT.V();
    }
}

void smoker2(void *arg)
{
    while(1){
        //printf("smoker2 thread running!\n");
        SMOKER_PAPER.P(); //smoker2 
        printf("smoker2: I am smoking!\n");
        AGENT.V();
    }
}

void smoker3(void *arg)
{
    while(1){
        //printf("smoker3 thread running!\n");
        SMOKER_TOBACCO.P(); //smoker3
        printf("smoker3: I am smoking!\n");
        AGENT.V();
    }
}




extern "C" void setup_kernel()
{

    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 创建第一个线程
    int pid1 = programManager.executeThread(producer, nullptr, "first thread", 1);
    int pid2 = programManager.executeThread(smoker1, nullptr, "second thread", 1);
    int pid3 = programManager.executeThread(smoker2, nullptr, "third thread", 1);
    int pid4 = programManager.executeThread(smoker3, nullptr, "fourth thread", 1);

    SMOKER_MATCH.initialize(0);
    SMOKER_PAPER.initialize(0);
    SMOKER_TOBACCO.initialize(0);
    AGENT.initialize(1);
    
    if (pid1 == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
