#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;

void first_thread(void *arg)
{
    //测试地址管理器是否正常工作
    uint32 a =memoryManager.allocatePhysicalPages(AddressPoolType::USER, 3);//分配一个页,大小为4KB
    uint32 b =memoryManager.allocatePhysicalPages(AddressPoolType::USER, 16000);//分配一个页,大小为4KB
    uint32 c =memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 1);//分配一个页,大小为4KB
    uint32 d =memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, 16000);//分配一个页,大小为4KB
    uint32 e =memoryManager.allocatePhysicalPages(AddressPoolType::USER, 2);//分配一个页,大小为4KB
    printf("trying to allocate 3 page in USER space and succeed at a: %x\n", a);
    printf("trying to allocate 16000 page in USER space and fail from b: %x\n", b);
    printf("trying to allocate 1 page in KERNEL space and succeed at c: %x\n", c);
    printf("trying to allocate 16000 page in kERNEL space and fail from d: %x\n", d);
    printf("trying to allocate 2 page in USER space and succeed at e: %x\n", e);
    asm_halt();
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

    // 内存管理器
    memoryManager.initialize();

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
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
