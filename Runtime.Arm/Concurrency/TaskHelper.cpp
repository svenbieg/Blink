//================
// TaskHelper.cpp
//================

#include "TaskHelper.h"


//=======
// Using
//=======

#include <assert.h>
#include <config.h>
#include <irq.h>
#include <task.h>
#include "Concurrency/Task.h"

extern BYTE __stack_end;


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID TaskHelper::Initialize(SIZE_T* StackPointer, VOID (*TaskProc)(VOID*), VOID* Parameter)noexcept
{
SIZE_T stack=*StackPointer;
stack-=sizeof(IRQ_FRAME);
auto irq_frame=(IRQ_FRAME*)stack;
irq_frame->PC=(SIZE_T)TaskProc;
irq_frame->R0=(SIZE_T)Parameter;
irq_frame->XPSR=(1<<24);
stack-=sizeof(TASK_FRAME);
*StackPointer=stack;
}

VOID TaskHelper::Switch(UINT core, Task* current, Task* next)noexcept
{
SIZE_T stack_end=(SIZE_T)&__stack_end;
auto irq_stack=(IRQ_STACK*)(stack_end-core*CONFIG_STACK_SIZE-sizeof(IRQ_STACK));
current->m_StackPointer=irq_stack->SP;
irq_stack->SP=next->m_StackPointer;
}

}