//==========
// Task.cpp
//==========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#task-creation

#include "Task.h"


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"
#include "Concurrency/Scheduler.h"
#include "Concurrency/TaskHelper.h"

using namespace Concurrency;


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

Handle<Task> Task::Create(VOID (*Procedure)(), Handle<String> Name, SIZE_T StackSize)
{
SIZE_T task_size=TypeHelper::AlignUp(sizeof(TaskProcedure), sizeof(SIZE_T));
auto task=(TaskProcedure*)MemoryHelper::Allocate(task_size+StackSize);
auto stack=(BYTE*)task+task_size;
new (task) TaskProcedure(stack, StackSize, Procedure, Name);
Schedule(task);
return task;
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

Handle<Task> Task::Get()
{
return Scheduler::GetCurrentTask();
}

BOOL Task::IsMainTask()
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

Task::Task(BYTE* stack, SIZE_T stack_size, Handle<String> name):
Cancelled(false),
Name(name->Begin()),
m_Creator(nullptr),
m_Exception(nullptr),
m_Flags(TaskFlags::None),
m_Name(name),
m_Next(nullptr),
m_PriorityCount(0),
m_ResumeTime(0),
m_Signal(nullptr),
m_StackBottom((SIZE_T)stack),
m_StackPointer((SIZE_T)stack+stack_size),
m_StackSize(stack_size),
m_Status(Status::Success),
m_Then(nullptr),
m_This(this)
{
TaskHelper::Initialize(&m_StackPointer, TaskProc, this);
}


//==================
// Common Protected
//==================

bool Task::Prepend(Task* first, Task* second)
{
if(FlagHelper::Get(second->m_Flags, TaskFlags::Priority))
	return false;
return true;
}

bool Task::Priority(Task* first, Task* second)
{
if(!FlagHelper::Get(first->m_Flags, TaskFlags::Priority))
	return false;
if(!FlagHelper::Get(second->m_Flags, TaskFlags::Priority))
	return true;
return false;
}

VOID Task::Schedule(Task* task)
{
Scheduler::AddTask(task);
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
if(task->m_Then)
	{
	DispatchedQueue::Append(task->m_Then);
	task->m_Then=nullptr;
	}
lock.Unlock();
task=nullptr;
Scheduler::ExitTask();
}


//==========================
// Con-/Destructors Private
//==========================

Task* Task::CreateInternal(VOID (*Procedure)(), Handle<String> Name, SIZE_T StackSize)
{
SIZE_T task_size=TypeHelper::AlignUp(sizeof(TaskProcedure), sizeof(SIZE_T));
auto task=(TaskProcedure*)MemoryHelper::Allocate(task_size+StackSize);
auto stack=(BYTE*)task+task_size;
new (task) TaskProcedure(stack, StackSize, Procedure, Name);
return task;
}

}