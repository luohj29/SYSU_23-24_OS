#ifndef PROGRAM_H
#define PROGRAM_H
#include "thread.h"
#include "list.h"

//�꺯��������
#define ListItem2PCB(ADDRESS, LIST_ITEM) ((PCB *)((int)(ADDRESS) - (int)&((PCB *)0)->LIST_ITEM))


class ProgramManager
{

    public:
    List allPrograms;   // ����״̬���߳�/���̵Ķ���
    List readyPrograms; // ����ready(����̬)���߳�/���̵Ķ���
    PCB *running;       // ��ǰִ�е��߳�

    public:
    ProgramManager();
    void initialize();

     // ����һ���̲߳������������
    // function���߳�ִ�еĺ���
    // parameter��ָ�����Ĳ�����ָ��
    // name���̵߳�����
    // priority���̵߳����ȼ�
    // �ɹ�������pid��ʧ�ܣ�����-1
    int executeThread(ThreadFunction function, void *parameter, const char *name, int priority);

    // ����һ��PCB
    PCB *allocatePCB();
    // �黹һ��PCB
    void releasePCB(PCB *program);

     // ִ���̵߳���
    void schedule();
};

void program_exit();

#endif