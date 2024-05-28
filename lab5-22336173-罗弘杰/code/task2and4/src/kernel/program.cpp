#include "program.h"
#include "stdlib.h"
#include "interrupt.h"
#include "asm_utils.h"
#include "stdio.h"
#include "thread.h"
#include "os_modules.h"

const int PCB_SIZE = 4096;                   // PCB的大小，4KB。
char PCB_SET[PCB_SIZE * MAX_PROGRAM_AMOUNT]; // 存放PCB的数组，预留了MAX_PROGRAM_AMOUNT个PCB的大小空间。
bool PCB_SET_STATUS[MAX_PROGRAM_AMOUNT];     // PCB的分配状态，true表示已经分配，false表示未分配。

ProgramManager::ProgramManager()
{
    initialize();
}

void ProgramManager::initialize()
{
    allPrograms.initialize();
    readyPrograms.initialize();
    running = nullptr;

    for (int i = 0; i < MAX_PROGRAM_AMOUNT; ++i)
    {
        PCB_SET_STATUS[i] = false;
    }
}
/*
    * 分配一个PCB
    * 遍历PCB_SET_STATUS数组，找到第一个未分配的PCB，将其分配，并返回PCB的地址
    * 如果没有未分配的PCB，返回nullptr
    */
PCB *ProgramManager::allocatePCB()
{
    for (int i = 0; i < MAX_PROGRAM_AMOUNT; i++)
    {
        if (PCB_SET_STATUS[i] == false)
        {
            PCB_SET_STATUS[i] = true;
            return (PCB *)(PCB_SET + i * PCB_SIZE);
        }
    }
    return nullptr;
}

/*
    * 线程退出
    * 1. 将当前线程的状态设置为DEAD
    * 2. 如果就绪队列中还有线程，调度下一个线程
    * 3. 如果就绪队列为空，关中断，输出halt，然后halt
    */
void program_exit()
{
    PCB *thread = programManager.running;
    thread->status = ProgramStatus::DEAD;

    if (!programManager.readyPrograms.empty())
    {
        programManager.schedule();
    }
    else
    {
        interruptManager.disableInterrupt();
        printf("halt\n");
        asm_halt();
    }
}


void ProgramManager::releasePCB(PCB *program)
{
    int index = ((int)program - (int)PCB_SET) / PCB_SIZE;
    PCB_SET_STATUS[index] = false;
}

/*
    * 创建一个线程并放入就绪队列
    * function：线程执行的函数
    * parameter：指向函数的参数的指针
    * name：线程的名称
    * priority：线程的优先级
    * 成功，返回pid；失败，返回-1
    */
int ProgramManager::executeThread(ThreadFunction function, void *parameter, const char *name, int priority)
{
    // 关中断，防止创建线程的过程被打断
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    // 分配一页作为PCB
    PCB *thread = allocatePCB();

    if (!thread)
        return -1;

    // 初始化分配的页
    memset(thread, 0, PCB_SIZE);

    for (int i = 0; i < MAX_PROGRAM_NAME && name[i]; ++i)
    {
        thread->name[i] = name[i];
    }

    thread->status = ProgramStatus::READY;
    thread->priority = priority;
    thread->ticks = priority * 10;
    thread->ticksPassedBy = 0;
    thread->pid = ((int)thread - (int)PCB_SET) / PCB_SIZE;

    // 线程栈
    thread->stack = (int *)((int)thread + PCB_SIZE);
    thread->stack -= 7;
    thread->stack[0] = 0; 
    thread->stack[1] = 0;
    thread->stack[2] = 0;
    thread->stack[3] = 0;
    thread->stack[4] = (int)function;  // 线程执行函数
    thread->stack[5] = (int)program_exit; // 线程退出函数
    thread->stack[6] = (int)parameter; // 参数
    
    //thread->tagInGeneralList是ListItem类型,他被初始化
    allPrograms.push_back(&(thread->tagInAllList)); 
    readyPrograms.insert_priority(&(thread->tagInGeneralList),thread->priority); // 按优先级插入,&(thread->tagInGeneralList)是ListItem类型

    // 恢复中断
    interruptManager.setInterruptStatus(status);

    return thread->pid;
}

/*
    * 线程调度
    * 1. 当前线程的状态是RUNNING，将其状态设置为READY，更新时间片，放入就绪队列
    * 2. 当前线程的状态是DEAD，释放PCB
    * 3. 从就绪队列中取出一个线程，设置为RUNNING，切换到该线程
    */
void ProgramManager::schedule()
{
    bool status = interruptManager.getInterruptStatus(); // 获取中断状态
    interruptManager.disableInterrupt(); // 关中断

    if (readyPrograms.size() == 0) // 没有线程可以调度
    {
        interruptManager.setInterruptStatus(status);
        return;
    }

    if (running->status == ProgramStatus::RUNNING) // 当前线程是RUNNING状态
    {
        running->status = ProgramStatus::READY;
        running->ticks = running->priority * 10; // 重新设置时间片
        readyPrograms.push_back(&(running->tagInGeneralList));
    }
    else if (running->status == ProgramStatus::DEAD) // 当前线程是DEAD状态
    {
        releasePCB(running);  // 释放PCB
    }

    // 取出下一个线程，设置为RUNNING状态
    ListItem *item = readyPrograms.front(); // 取出就绪队列的第一个,也就是优先级最高的
    PCB *next = ListItem2PCB(item, tagInGeneralList);  // 将ListItem转换为PCB
    PCB *cur = running;
    next->status = ProgramStatus::RUNNING;
    running = next;
    readyPrograms.pop_front();

    asm_switch_thread(cur, next); // 切换线程

    interruptManager.setInterruptStatus(status);  // 恢复中断
}