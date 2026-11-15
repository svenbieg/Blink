//==========
// Task.cpp
//==========

// Separate execution context for multi-threading.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#task-creation

#include "Task.h"


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"
#include "Concurrency/Scheduler.h"
#include "Concurrency/TaskHelper.h"
#include "StatusHelper.h"


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Con-/Destructors
//==================

Task::~Task()noexcept
{
if(m_Exception)
	{
	delete m_Exception;
	m_Exception=nullptr;
	}
}

Handle<Task> Task::Create(VOID (*Procedure)(), Handle<String> Name, SIZE_T StackSize)
{
auto task=TaskProcedure::Create(Procedure, std::forward<Handle<String>>(Name), StackSize);
Scheduler::AddTask(task);
return task;
}


//========
// Common
//========

VOID Task::Cancel()noexcept
{
Scheduler::CancelTask(this);
}

BOOL Task::Cancelled()
{
SpinLock lock(Scheduler::s_CriticalSection);
return StatusHelper::Failed(m_Status);
}

Handle<Task> Task::Get()noexcept
{
return Scheduler::GetCurrentTask();
}

BOOL Task::IsMainTask()noexcept
{
auto current=Scheduler::GetCurrentTask();
return current==Scheduler::s_MainTask;
}

VOID Task::Sleep(UINT ms)
{
assert(!Task::IsMainTask());
Scheduler::SuspendCurrentTask(ms);
}


//============================
// Con-/Destructors Protected
//============================

Task::Task(BYTE* stack, SIZE_T stack_size, Handle<String>&& name)noexcept:
Name(name->Begin()),
m_Creator(nullptr),
m_Exception(nullptr),
m_Flags(TaskFlags::None),
m_Name(std::move(name)),
m_Next(nullptr),
m_Owner(nullptr),
m_PriorityCount(0),
m_ResumeTime(0),
m_SignalCount(0),
m_SignalLock(nullptr),
m_StackBottom((SIZE_T)stack),
m_StackPointer((SIZE_T)stack+stack_size),
m_StackUsed(0),
m_StackSize(stack_size),
m_StackTop(m_StackPointer),
m_StartTime(0),
m_Status(Status::Success),
m_Then(nullptr),
m_This(this),
m_TotalTime(0),
z_StackHeader(STACK_HEADER)
{
TaskHelper::Initialize(&m_StackPointer, TaskProc, this);
}


//==================
// Common Protected
//==================

bool Task::Priority(Task* first, Task* second)noexcept
{
if(FlagHelper::Get(second->m_Flags, TaskFlags::Priority))
	return false;
return true;
}

VOID Task::Schedule(Task* task)noexcept
{
Scheduler::AddTask(task);
}

VOID Task::TaskProc(VOID* param)
{
auto task=(Task*)param;
Status status=Status::Success;
try
	{
	task->Run();
	status=task->m_Status;
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
if(task->m_Then)
	{
	DispatchedQueue::Append(task->m_Then);
	task->m_Then=nullptr;
	}
Scheduler::ExitTask(status);
}

VOID Task::ThrowIfFailed(Task* task)
{
auto status=task->m_Status;
if(StatusHelper::Failed(status))
	{
	task->m_Status=Status::Success;
	StatusHelper::ThrowIfFailed(status);
	}
}

VOID Task::ThrowIfFailed(Task* task, SpinLock& sched_lock)
{
auto status=task->m_Status;
if(StatusHelper::Failed(status))
	{
	task->m_Status=Status::Success;
	sched_lock.Unlock();
	StatusHelper::ThrowIfFailed(status);
	}
}


//==========================
// Con-/Destructors Private
//==========================

Handle<Task> Task::CreateInternal(VOID (*Procedure)(), Handle<String> Name, SIZE_T StackSize)
{
return TaskProcedure::Create(Procedure, std::forward<Handle<String>>(Name), StackSize);
}

}