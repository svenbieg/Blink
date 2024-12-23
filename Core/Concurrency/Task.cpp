//==========
// Task.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "Devices/Timers/SystemTimer.h"
#include "Scheduler.h"
#include "Task.h"

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
ScopedLock lock(m_Mutex);
m_Then=nullptr;
Cancelled=true;
}

Handle<Task> Task::Get()
{
return Scheduler::GetCurrentTask();
}

VOID Task::Sleep(UINT ms)
{
assert(!Scheduler::IsMainTask()); // Sleeping is not allowed in the main-task
Scheduler::SuspendCurrentTask(ms);
}

VOID Task::SleepMicroseconds(UINT us)
{
assert(!Scheduler::IsMainTask()); // Sleeping is not allowed in the main-task
UINT64 end=SystemTimer::Microseconds64()+us;
while(SystemTimer::Microseconds64()<=end);
}

Status Task::Wait()
{
assert(!Scheduler::IsMainTask()); // Waiting is not allowed in the main-task
ScopedLock lock(m_Mutex);
if(m_Status!=Status::Pending)
	return m_Status;
m_Done.Wait(lock);
return m_Status;
}


//============================
// Con-/Destructors Protected
//============================

Task::Task():
Cancelled(false),
m_Status(Status::Pending),
// Private
m_BlockingCount(0),
m_Flags(TaskFlags::None),
m_ResumeTime(0),
m_StackPointer(&m_Stack[STACK_SIZE]),
m_Then(nullptr)
{}


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