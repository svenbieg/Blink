//================
// TaskHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include <config.h>
#include <irq.h>
#include "Concurrency/Task.h"
#include "Concurrency/TaskHelper.h"

extern BYTE __stack_end;


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID TaskHelper::Switch(UINT core, Task* current, Task* next)
{
SIZE_T stack_end=(SIZE_T)&__stack_end;
auto irq_stack=(IRQ_STACK*)(stack_end-core*STACK_SIZE-sizeof(IRQ_STACK));
assert(irq_stack->SP-sizeof(TASK_FRAME)>=current->m_StackBottom); // Stack-overflow
current->m_StackPointer=SaveContext(irq_stack->SP);
irq_stack->SP=RestoreContext(next->m_StackPointer);
}

}