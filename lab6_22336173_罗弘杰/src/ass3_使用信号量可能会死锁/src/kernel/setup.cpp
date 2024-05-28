#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 灞忓箷IO澶勭悊鍣?
STDIO stdio;
// 涓柇绠＄悊鍣?
InterruptManager interruptManager;
// 绋嬪簭绠＄悊鍣?
ProgramManager programManager;

Semaphore CH1;
Semaphore CH2;
Semaphore CH3;
Semaphore CH4;
Semaphore CH5;



//模拟5个哲学家，5根筷子吃饭问题,使用5个信号量对应5根筷子，可能会发生死锁
void PH1(void *arg)
{   
    int time = 0;
    while (1)
    {
        CH1.P();
        CH2.P();
        int wait = 0x0fffffff;
        printf("22336173_LHJ is eating: %d\n", ++time);
        while (wait--)
            ;
        CH1.V();
        CH2.V();
        wait = 0x0ffffff;
        while (wait--)
            ;
        //printf("22336173_LHJ is thinking\n");
    }
}

void PH2(void *arg)
{   
    int time = 0;
    while (1)
    {
        CH2.P();
        CH3.P();
        printf("PH2 is eating: %d\n", ++time);
        int wait = 0x0fffffff;
         while (wait--)
            ;
        CH2.V();
        CH3.V();
        //printf("PH2 is thinking\n");
        wait = 0x0fffffff;
         while (wait--)
            ;
    }
}

void PH3(void *arg)
{   
    int time = 0;
    while (1)
    {
        CH3.P();
        CH4.P();
        printf("PH3 is eating: %d\n", ++time);
        int wait = 0x0fffffff;
         while (wait--)
            ;        

        CH3.V();
        CH4.V();
        //printf("PH3 is thinking\n");
        wait = 0x0fffffff;
         while (wait--)
            ;
    }
}

void PH4(void *arg)
{   
    int time = 0;
    while (1)
    {
        CH4.P();
        CH5.P();
        printf("PH4 is eating: %d\n", ++time);
        int wait = 0x0fffffff;
         while (wait--)
            ;
        CH4.V();
        CH5.V();
        //printf("PH4 is thinking\n");
        wait = 0x0fffffff;
         while (wait--)
            ;
    }
}

void PH5(void *arg)
{   
    int time = 0;
    while (1)
    {
        CH5.P();
        CH1.P();
        printf("PH5 is eating: %d\n", ++time);
        int wait = 0x0fffffff;
         while (wait--)
            ;
        CH5.V();
        CH1.V();
        //printf("PH5 is thinking\n");
        wait = 0x0fffffff;
         while (wait--)
            ;
    }
}




extern "C" void setup_kernel()
{

    // 涓柇绠＄悊鍣?
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 杈撳嚭绠＄悊鍣?
    stdio.initialize();

    // 杩涚▼/绾跨▼绠＄悊鍣?
    programManager.initialize();

    // 鍒涘缓绗竴涓嚎绋?
    int pid1 = programManager.executeThread(PH1, nullptr, "first thread", 1);
    int pid2 = programManager.executeThread(PH2, nullptr, "second thread", 1);
    int pid3 = programManager.executeThread(PH3, nullptr, "third thread", 1);
    int pid4 = programManager.executeThread(PH4, nullptr, "fourth thread", 1);
    int pid5 = programManager.executeThread(PH5, nullptr, "fifth thread", 1);

    CH1.initialize(1);
    CH2.initialize(1);
    CH3.initialize(1);
    CH4.initialize(1);
    CH5.initialize(1);

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
