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

VOID TaskHelper::Initialize(SIZE_T* StackPointer, VOID (*TaskProc)(VOID*), VOID* Parameter)
{
task_init(StackPointer, TaskProc, Parameter);
}

SIZE_T TaskHelper::RestoreContext(SIZE_T StackPointer)
{
return task_restore_context(StackPointer);
}

SIZE_T TaskHelper::SaveContext(SIZE_T StackPointer)
{
return task_save_context(StackPointer);
}

VOID TaskHelper::Switch(UINT core, Task* current, Task* next)
{
SIZE_T stack_end=(SIZE_T)&__stack_end;
auto irq_stack=(IRQ_STACK*)(stack_end-core*CONFIG_STACK_SIZE-sizeof(IRQ_STACK));
assert(irq_stack->SP-sizeof(TASK_FRAME)>=current->m_StackBottom); // Stack-overflow
current->m_StackPointer=SaveContext(irq_stack->SP);
irq_stack->SP=RestoreContext(next->m_StackPointer);
}

}