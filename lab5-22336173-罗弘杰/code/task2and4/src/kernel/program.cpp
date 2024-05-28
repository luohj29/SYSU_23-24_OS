#include "program.h"
#include "stdlib.h"
#include "interrupt.h"
#include "asm_utils.h"
#include "stdio.h"
#include "thread.h"
#include "os_modules.h"

const int PCB_SIZE = 4096;                   // PCB�Ĵ�С��4KB��
char PCB_SET[PCB_SIZE * MAX_PROGRAM_AMOUNT]; // ���PCB�����飬Ԥ����MAX_PROGRAM_AMOUNT��PCB�Ĵ�С�ռ䡣
bool PCB_SET_STATUS[MAX_PROGRAM_AMOUNT];     // PCB�ķ���״̬��true��ʾ�Ѿ����䣬false��ʾδ���䡣

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
    * ����һ��PCB
    * ����PCB_SET_STATUS���飬�ҵ���һ��δ�����PCB��������䣬������PCB�ĵ�ַ
    * ���û��δ�����PCB������nullptr
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
    * �߳��˳�
    * 1. ����ǰ�̵߳�״̬����ΪDEAD
    * 2. ������������л����̣߳�������һ���߳�
    * 3. �����������Ϊ�գ����жϣ����halt��Ȼ��halt
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
    * ����һ���̲߳������������
    * function���߳�ִ�еĺ���
    * parameter��ָ�����Ĳ�����ָ��
    * name���̵߳�����
    * priority���̵߳����ȼ�
    * �ɹ�������pid��ʧ�ܣ�����-1
    */
int ProgramManager::executeThread(ThreadFunction function, void *parameter, const char *name, int priority)
{
    // ���жϣ���ֹ�����̵߳Ĺ��̱����
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    // ����һҳ��ΪPCB
    PCB *thread = allocatePCB();

    if (!thread)
        return -1;

    // ��ʼ�������ҳ
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

    // �߳�ջ
    thread->stack = (int *)((int)thread + PCB_SIZE);
    thread->stack -= 7;
    thread->stack[0] = 0; 
    thread->stack[1] = 0;
    thread->stack[2] = 0;
    thread->stack[3] = 0;
    thread->stack[4] = (int)function;  // �߳�ִ�к���
    thread->stack[5] = (int)program_exit; // �߳��˳�����
    thread->stack[6] = (int)parameter; // ����
    
    //thread->tagInGeneralList��ListItem����,������ʼ��
    allPrograms.push_back(&(thread->tagInAllList)); 
    readyPrograms.insert_priority(&(thread->tagInGeneralList),thread->priority); // �����ȼ�����,&(thread->tagInGeneralList)��ListItem����

    // �ָ��ж�
    interruptManager.setInterruptStatus(status);

    return thread->pid;
}

/*
    * �̵߳���
    * 1. ��ǰ�̵߳�״̬��RUNNING������״̬����ΪREADY������ʱ��Ƭ�������������
    * 2. ��ǰ�̵߳�״̬��DEAD���ͷ�PCB
    * 3. �Ӿ���������ȡ��һ���̣߳�����ΪRUNNING���л������߳�
    */
void ProgramManager::schedule()
{
    bool status = interruptManager.getInterruptStatus(); // ��ȡ�ж�״̬
    interruptManager.disableInterrupt(); // ���ж�

    if (readyPrograms.size() == 0) // û���߳̿��Ե���
    {
        interruptManager.setInterruptStatus(status);
        return;
    }

    if (running->status == ProgramStatus::RUNNING) // ��ǰ�߳���RUNNING״̬
    {
        running->status = ProgramStatus::READY;
        running->ticks = running->priority * 10; // ��������ʱ��Ƭ
        readyPrograms.push_back(&(running->tagInGeneralList));
    }
    else if (running->status == ProgramStatus::DEAD) // ��ǰ�߳���DEAD״̬
    {
        releasePCB(running);  // �ͷ�PCB
    }

    // ȡ����һ���̣߳�����ΪRUNNING״̬
    ListItem *item = readyPrograms.front(); // ȡ���������еĵ�һ��,Ҳ�������ȼ���ߵ�
    PCB *next = ListItem2PCB(item, tagInGeneralList);  // ��ListItemת��ΪPCB
    PCB *cur = running;
    next->status = ProgramStatus::RUNNING;
    running = next;
    readyPrograms.pop_front();

    asm_switch_thread(cur, next); // �л��߳�

    interruptManager.setInterruptStatus(status);  // �ָ��ж�
}