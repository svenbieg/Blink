//==========
// Task.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/TaskLock.h"
#include "Devices/Timers/SystemTimer.h"
#include "Scheduler.h"
#include "SpinLock.h"
#include "Task.h"

using namespace Concurrency;
using namespace Devices::Timers;


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

VOID Task::Lock()
{
SpinLock lock(Scheduler::s_CriticalSection);
FlagHelper::Set(m_Flags, TaskFlags::Locked);
m_LockCount++;
}

VOID Task::Sleep(UINT ms)
{
assert(!Task::IsMainTask());
Scheduler::SuspendCurrentTask(ms);
}

VOID Task::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(--m_LockCount==0)
	FlagHelper::Clear(m_Flags, TaskFlags::Locked);
}

Status Task::Wait()
{
Task::ThrowIfMain();
WriteLock lock(m_Mutex);
if(GetFlag(TaskFlags::Done))
	return m_Status;
m_Done.Wait(lock);
return m_Status;
}


//============================
// Con-/Destructors Protected
//============================

Task::Task(Handle<String> name, VOID* stack_end, UINT stack_size):
Cancelled(false),
m_Exception(nullptr),
m_Flags(TaskFlags::None),
m_LockCount(0),
m_Name(name),
m_ResumeTime(0),
m_StackPointer(stack_end),
m_StackSize(stack_size),
m_Status(Status::Pending)
{
TaskHelper::Initialize(&m_StackPointer, TaskProc, this);
}


//==================
// Common Protected
//==================

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
catch(Status s)
	{
	status=s;
	}
catch(...)
	{
	status=Status::Error;
	}
WriteLock lock(task->m_Mutex);
task->SetFlag(TaskFlags::Done);
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