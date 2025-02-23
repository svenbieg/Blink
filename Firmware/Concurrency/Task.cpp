//==========
// Task.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include <irq.h>
#include <task.h>
#include "Concurrency/TaskLock.h"
#include "Devices/Timers/SystemTimer.h"
#include "Scheduler.h"
#include "Task.h"

using namespace Concurrency;
using namespace Devices::Timers;

extern BYTE __stack_end;


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Con-/Destructors
//==================

Task::~Task()
{
if(m_Exception)
	{
	delete m_Exception;
	m_Exception=nullptr;
	}
}


//========
// Common
//========

VOID Task::Cancel()
{
TaskLock lock(m_Mutex);
m_Then=nullptr;
Cancelled=true;
}

Handle<Task> Task::Get()
{
return Scheduler::GetCurrentTask();
}

VOID Task::Sleep(UINT ms)
{
Task::ThrowIfMain();
Scheduler::SuspendCurrentTask(ms);
}

VOID Task::SleepMicroseconds(UINT us)
{
Task::ThrowIfMain();
UINT64 end=SystemTimer::Microseconds64()+us;
while(SystemTimer::Microseconds64()<=end);
}

Status Task::Wait()
{
Task::ThrowIfMain();
ScopedLock lock(m_Mutex);
if(m_Status!=Status::Pending)
	return m_Status;
m_Done.Wait(lock);
return m_Status;
}


//============================
// Con-/Destructors Protected
//============================

Task::Task(Handle<String> name, VOID* stack_end, UINT stack_size):
Cancelled(false),
m_BlockingCount(0),
m_Exception(nullptr),
m_Flags(TaskFlags::None),
m_Name(name),
m_ResumeTime(0),
m_StackPointer(stack_end),
m_StackSize(stack_size),
m_Status(Status::Pending)
{
task_init(&m_StackPointer, TaskProc, this);
}


//==================
// Common Protected
//==================

VOID Task::Switch(UINT core, Task* current, Task* next)
{
SIZE_T stack_end=(SIZE_T)&__stack_end;
auto irq_stack=(IRQ_STACK*)(stack_end-core*STACK_SIZE-sizeof(IRQ_STACK));
assert(irq_stack->SP-sizeof(TASK_FRAME)>=(SIZE_T)current+sizeof(Task)); // Stack-overflow
current->m_StackPointer=task_save_context((VOID*)irq_stack->SP);
irq_stack->SP=task_restore_context(next->m_StackPointer);
}

VOID Task::TaskProc(VOID* param)
{
Handle<Task> task=(Task*)param;
Status status=Status::Success;
try
	{
	task->Run();
	}
catch(Exception e)
	{
	status=e.GetStatus();
	}
catch(...)
	{
	status=Status::Error;
	}
ScopedLock lock(task->m_Mutex);
task->m_Status=status;
task->m_Done.Trigger();
if(task->m_Then)
	{
	DispatchedQueue::Append(task->m_Then);
	task->m_Then=nullptr;
	}
lock.Unlock();
task=nullptr;
Scheduler::ExitTask();
}

}