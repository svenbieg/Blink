//==========
// Task.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Concurrency/SpinLock.h"
#include "Concurrency/Task.h"
#include "Devices/Timers/SystemTimer.h"
#include "FlagHelper.h"

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
WriteLock lock(m_Mutex);
if(FlagHelper::Get(m_Flags, TaskFlags::Done))
	return;
Cancelled=true;
m_Status=Status::Aborted;
Scheduler::CancelTask(this);
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

Status Task::Wait()
{
assert(!Task::IsMainTask());
WriteLock lock(m_Mutex);
if(FlagHelper::Get(m_Flags, TaskFlags::Done))
	return m_Status;
m_Done.Wait(lock);
return m_Status;
}


//============================
// Con-/Destructors Protected
//============================

Task::Task(Handle<String> name, VOID* stack_end, UINT stack_size):
Cancelled(false),
Name(name->Begin()),
m_Create(nullptr),
m_Exception(nullptr),
m_Flags(TaskFlags::None),
m_LockCount(0),
m_Name(name),
m_Next(nullptr),
m_Owner(nullptr),
m_Parallel(nullptr),
m_Release(nullptr),
m_ResumeTime(0),
m_Signal(nullptr),
m_Sleeping(nullptr),
m_StackPointer(stack_end),
m_StackSize(stack_size),
m_Status(Status::Success),
m_Then(nullptr),
m_Waiting(nullptr)
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
FlagHelper::Set(task->m_Flags, TaskFlags::Done);
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