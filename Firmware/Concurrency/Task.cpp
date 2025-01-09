//==========
// Task.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include <task.h>
#include "Concurrency/TaskLock.h"
#include "Devices/Timers/SystemTimer.h"
#include "Scheduler.h"
#include "Task.h"

using namespace Concurrency;
using namespace Devices::Timers;


//===========
// Namespace
//===========

namespace Concurrency {


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
m_Status(Status::Pending),
// Private
m_BlockingCount(0),
m_Flags(TaskFlags::None),
m_Name(name),
m_ResumeTime(0),
m_StackPointer(stack_end),
m_StackSize(stack_size)
{
task_init(&m_StackPointer, TaskProc, this);
}


//================
// Common Private
//================

VOID Task::TaskProc(VOID* param)
{
Handle<Task> task=(Task*)param;
Status status=Status::Success;
try
	{
	task->Run();
	}
catch(Exception& e)
	{
	status=e.GetStatus();
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