#ifndef THREAD_H
#define THREAD_H

#include "list.h"
#include "os_constant.h"

typedef void (*ThreadFunction)(void *);

enum ProgramStatus
{
    CREATED,
    RUNNING,
    READY,
    BLOCKED,
    DEAD
};

struct PCB
{
    int *stack;                      // ջָ�룬���ڵ���ʱ����esp
    char name[MAX_PROGRAM_NAME + 1]; // �߳���
    enum ProgramStatus status;       // �̵߳�״̬
    int priority;                    // �߳����ȼ�
    int pid;                         // �߳�pid
    int ticks;                       // �߳�ʱ��Ƭ��ʱ��
    int ticksPassedBy;               // �߳���ִ��ʱ��
    ListItem tagInGeneralList;       // �̶߳��б�ʶ�����ھ������к���������
    ListItem tagInAllList;           // �̶߳��б�ʶ�� ���������̶߳���
};

#endif