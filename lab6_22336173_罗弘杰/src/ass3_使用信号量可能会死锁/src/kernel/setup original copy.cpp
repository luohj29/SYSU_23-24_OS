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

Semaphore semaphore;

enum Material
{
    TOBACCO = 1,
    PAPER = 2,
    MATCH= 4
};

int TOBACCO_NUM = 0, PAPER_NUM = 0, MATCH_NUM = 0;
int SIGNAL =1;

int get_random(int x){
    int rand = asm_random();
    return rand % x;
}
void producer(void *arg)
{   
    int rand = 0;
    //static const int choices[3] = {TOBACCO|MATCH, PAPER|TOBACCO, MATCH|PAPER};
    //static const int matching[3] = {PAPER, TOBACCO, MATCH};
    while(1){
        //printf("producer thread running!\n");
        while(SIGNAL){
        int rand = (rand +1)% 3;
        printf("offer material %d\n", rand+1);
        if (rand == 0){
            TOBACCO_NUM++;
            MATCH_NUM++;
        }else if (rand == 1){
            PAPER_NUM++;
            TOBACCO_NUM++;
        }else{
            MATCH_NUM++;
            PAPER_NUM++;
        }
        SIGNAL = 0;
    }
}
    
}
void smoker1(void *arg)
{   
    bool TOBACCO_FLAG = false;
    bool MATCH_FLAG = false;
    while(1){
        //printf("smoker1 thread running!\n");
        if (TOBACCO_NUM&&MATCH_NUM){
            TOBACCO_NUM=0;
            TOBACCO_FLAG = true;
            MATCH_NUM=0;
            MATCH_FLAG = true;
            printf("smoker_have_paper: GET MATCH!\n");
            printf("smoker_have_paper: GET TOBACCO!\n");
        }

        if (TOBACCO_FLAG && MATCH_FLAG){
            printf("smoker_have_paper: I am smoking!\n");
            SIGNAL++;
            //printf("waiting for producer!\n");
            TOBACCO_FLAG = false;
            MATCH_FLAG = false;
        }
    }
}

void smoker2(void *arg)
{
    bool TOBACCO_FLAG = false;
    bool PAPER_FLAG = false;
    while(1){
        //printf("smoker2 thread running!\n");
        if (TOBACCO_NUM&&PAPER_NUM){
            PAPER_NUM=0;
            PAPER_FLAG = true;
            printf("smoker_have_match: GET PAPER!\n");
            TOBACCO_NUM=0;
            TOBACCO_FLAG = true;
            printf("smoker_have_match: GET TOBACCO!\n");
        }
       if (TOBACCO_FLAG && PAPER_FLAG){
            printf("smoker_have_match: I am smoking!\n");
            SIGNAL++;
            //printf("waiting for producer!\n");
            TOBACCO_FLAG = false;
            PAPER_FLAG = false;
        }
    }
}

void smoker3(void *arg)
{   
    bool PAPER_FLAG = false;
    bool MATCH_FLAG = false;
    while(1){
        //printf("smoker3 thread running!\n");
        if (PAPER_NUM&&MATCH_NUM){
            MATCH_NUM=0;
            MATCH_FLAG = true;
            printf("smoker_have_tobacco: GET MATCH!\n");
            PAPER_NUM=0;
            PAPER_FLAG = true;
            printf("smoker_have_tobacco: GET PAPER!\n");
        }
        if (PAPER_FLAG && MATCH_FLAG){
            printf("smoker_have_tobacco: I am smoking!\n");
            SIGNAL++;
            //printf("waiting for producer!\n");
            PAPER_FLAG = false;
            MATCH_FLAG = false;
        }
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
