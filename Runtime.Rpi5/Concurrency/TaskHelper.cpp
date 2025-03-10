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
#include <task.h>
#include "Concurrency/Task.h"
#include "TaskHelper.h"

extern BYTE __stack_end;


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID TaskHelper::Initialize(VOID** stack, VOID (*proc)(VOID*), VOID* param)
{
task_init(stack, proc, param);
}

SIZE_T TaskHelper::RestoreContext(VOID* stack)
{
return task_restore_context(stack);
}

VOID* TaskHelper::SaveContext(VOID* stack)
{
return task_save_context(stack);
}

VOID TaskHelper::Switch(UINT core, Task* current, Task* next)
{
SIZE_T stack_end=(SIZE_T)&__stack_end;
auto irq_stack=(IRQ_STACK*)(stack_end-core*STACK_SIZE-sizeof(IRQ_STACK));
assert(irq_stack->SP-sizeof(TASK_FRAME)>=(SIZE_T)current+sizeof(Task)); // Stack-overflow
current->m_StackPointer=SaveContext((VOID*)irq_stack->SP);
irq_stack->SP=RestoreContext(next->m_StackPointer);
}

}