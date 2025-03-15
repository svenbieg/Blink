//========
// Task.h
//========

#pragma once


//=======
// Using
//=======

#include <assert.h>
#include <new>
#include <unwind.h>
#include "Concurrency/TaskHelper.h"
#include "Concurrency/DispatchedQueue.h"
#include "Concurrency/Scheduler.h"
#include "Concurrency/ReadLock.h"
#include "Concurrency/Signal.h"
#include "Concurrency/TaskLock.h"
#include "Concurrency/WriteLock.h"
#include "Exception.h"
#include "Status.h"


//===========
// Namespace
//===========

namespace Concurrency {


//=======
// Flags
//=======

enum class TaskFlags: UINT
{
None=0,
Locked=1,
Switch=2,
Owner=4,
Busy=7,
Sharing=8,
Suspended=16,
Done=32
};


//======
// Task
//======

class Task: public Object
{
public:
	// Friends
	friend Mutex;
	friend Scheduler;
	friend Signal;
	friend TaskHelper;
	friend UnwindException;

	// Con-/Destructors
	~Task();
	static Handle<Task> Create(VOID (*Procedure)(), Handle<String> Name="task", UINT StackSize=STACK_SIZE);
	template <class _owner_t> static Handle<Task> Create(_owner_t* Owner, VOID (_owner_t::*Procedure)(), Handle<String> Name="task", UINT StackSize=STACK_SIZE);
	template <class _owner_t> static Handle<Task> Create(Handle<_owner_t> const& Owner, VOID (_owner_t::*Procedure)(), Handle<String> Name="task", UINT StackSize=STACK_SIZE);
	template <class _owner_t, class _lambda_t> static Handle<Task> Create(_owner_t* Owner, _lambda_t&& Lambda, Handle<String> Name="task", UINT StackSize=STACK_SIZE);
	template <class _owner_t, class _lambda_t> static Handle<Task> Create(Handle<_owner_t> const& Owner, _lambda_t&& Lambda, Handle<String> Name="task", UINT StackSize=STACK_SIZE);

	// Common
	VOID Cancel();
	volatile BOOL Cancelled;
	static inline Handle<Task> Get() { return Scheduler::GetCurrentTask(); }
	inline Status GetStatus()const { return m_Status; }
	static inline BOOL IsMainTask() { return Scheduler::IsMainTask(); }
	VOID Lock();
	Handle<Object> Result;
	static VOID Sleep(UINT Milliseconds);
	inline VOID Then(VOID (*Procedure)())
		{
		DispatchedHandler::Append(&m_Then, new DispatchedProcedure(Procedure));
		}
	template <class _owner_t> inline VOID Then(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		DispatchedHandler::Append(&m_Then, new DispatchedMemberProcedure<_owner_t>(Owner, Procedure));
		}
	template <class _owner_t, class... _args_t> inline VOID Then(Handle<_owner_t> const& Owner, VOID (_owner_t::*Procedure)())
		{
		DispatchedHandler::Append(&m_Then, new DispatchedMemberProcedure<_owner_t>(Owner, Procedure));
		}
	template <class _owner_t, class _lambda_t> inline VOID Then(_owner_t* Owner, _lambda_t&& Lambda)
		{
		DispatchedHandler::Append(&m_Then, new DispatchedLambda(Owner, std::forward<_lambda_t>(Lambda)));
		}
	template <class _owner_t, class _lambda_t> inline VOID Then(Handle<_owner_t> const& Owner, _lambda_t&& Lambda)
		{
		DispatchedHandler::Append(&m_Then, new DispatchedLambda(Owner, std::forward<_lambda_t>(Lambda)));
		}
	static inline VOID ThrowIfMain()
		{
		if(Scheduler::IsMainTask())
			throw InvalidContextException();
		}
	VOID Unlock();
	Status Wait();

protected:
	// Con-/Destructors
	Task(Handle<String> Name, VOID* StackEnd, UINT StackSize);
	static Handle<Task> CreateInternal(VOID (*Procedure)(), Handle<String> Name, UINT StackSize=PAGE_SIZE);

	// Common
	virtual VOID Run()=0;
	static VOID TaskProc(VOID* Parameter);
	Signal m_Done;
	UnwindException* m_Exception;
	TaskFlags m_Flags;
	UINT m_LockCount;
	Mutex m_Mutex;
	Handle<String> m_Name;
	Handle<Task> m_Next;
	Handle<Task> m_Owner;
	Handle<Task> m_Parallel;
	UINT64 m_ResumeTime;
	Signal* m_Signal;
	VOID* m_StackPointer;
	UINT m_StackSize;
	Status m_Status;
	Handle<DispatchedHandler> m_Then;
	Handle<Task> m_Waiting;
};


//===========
// Procedure
//===========

class TaskProcedure: public Task
{
public:
	// Using
	typedef VOID (*_proc_t)();

	// Con-/Destructors
	TaskProcedure(_proc_t Procedure, Handle<String> Name, VOID* StackEnd, UINT StackSize):
		Task(Name, StackEnd, StackSize),
		m_Procedure(Procedure)
		{}

private:
	// Common
	VOID Run()override { m_Procedure(); }
	_proc_t m_Procedure;
};


//==================
// Member-Procedure
//==================

template <class _owner_t> class TaskMemberProcedure: public Task
{
public:
	// Using
	typedef VOID (_owner_t::*_proc_t)();

	// Con-/Destructors
	TaskMemberProcedure(_owner_t* Owner, _proc_t Procedure, Handle<String> Name, VOID* StackEnd, UINT StackSize):
		Task(Name, StackEnd, StackSize),
		m_Owner(Owner),
		m_Procedure(Procedure)
		{}

private:
	// Common
	VOID Run()override { (m_Owner->*m_Procedure)(); }
	Handle<_owner_t> m_Owner;
	_proc_t m_Procedure;
};


//=============
// Task-Lambda
//=============

template <class _owner_t, class _lambda_t> class TaskLambda: public Task
{
public:
	// Con-/Destructors
	TaskLambda(_owner_t* Owner, _lambda_t&& Lambda, Handle<String> Name, VOID* StackEnd, UINT StackSize):
		Task(Name, StackEnd, StackSize),
		m_Lambda(std::move(Lambda)),
		m_Owner(Owner)
		{}

private:
	// Common
	VOID Run()override { m_Lambda(); }
	_lambda_t m_Lambda;
	Handle<_owner_t> m_Owner;
};


//==================
// Con-/Destructors
//==================

inline Handle<Task> Task::Create(VOID (*Procedure)(), Handle<String> Name, UINT StackSize)
{
assert(StackSize%sizeof(SIZE_T)==0);
UINT size=TypeHelper::AlignUp(sizeof(TaskProcedure), sizeof(SIZE_T))+StackSize;
auto task=(TaskProcedure*)operator new(size);
VOID* stack_end=(VOID*)((SIZE_T)task+size);
new (task) TaskProcedure(Procedure, Name, stack_end, StackSize);
Scheduler::AddTask(task);
return task;
}

template <class _owner_t> inline Handle<Task> Task::Create(_owner_t* Owner, VOID (_owner_t::*Procedure)(), Handle<String> Name, UINT StackSize)
{
assert(StackSize%sizeof(SIZE_T)==0);
using _task_t=TaskMemberProcedure<_owner_t>;
UINT size=TypeHelper::AlignUp(sizeof(_task_t), sizeof(SIZE_T))+StackSize;
auto task=(_task_t*)operator new(size);
VOID* stack_end=(VOID*)((SIZE_T)task+size);
new (task) _task_t(Owner, Procedure, Name, stack_end, StackSize);
Scheduler::AddTask(task);
return task;
}

template <class _owner_t> inline Handle<Task> Task::Create(Handle<_owner_t> const& Owner, VOID (_owner_t::*Procedure)(), Handle<String> Name, UINT StackSize)
{
assert(StackSize%sizeof(SIZE_T)==0);
using _task_t=TaskMemberProcedure<_owner_t>;
UINT size=TypeHelper::AlignUp(sizeof(_task_t), sizeof(SIZE_T))+StackSize;
auto task=(_task_t*)operator new(size);
VOID* stack_end=(VOID*)((SIZE_T)task+size);
new (task) _task_t(Owner, Procedure, Name, stack_end, StackSize);
Scheduler::AddTask(task);
return task;
}

template <class _owner_t, class _lambda_t> inline Handle<Task> Task::Create(_owner_t* Owner, _lambda_t&& Lambda, Handle<String> Name, UINT StackSize)
{
assert(StackSize%sizeof(SIZE_T)==0);
using _task_t=TaskLambda<_owner_t, _lambda_t>;
UINT size=TypeHelper::AlignUp(sizeof(_task_t), sizeof(SIZE_T))+StackSize;
auto task=(_task_t*)operator new(size);
VOID* stack_end=(VOID*)((SIZE_T)task+size);
new (task) _task_t(Owner, std::forward<_lambda_t>(Lambda), Name, stack_end, StackSize);
Scheduler::AddTask(task);
return task;
}

template <class _owner_t, class _lambda_t> inline Handle<Task> Task::Create(Handle<_owner_t> const& Owner, _lambda_t&& Lambda, Handle<String> Name, UINT StackSize)
{
assert(StackSize%sizeof(SIZE_T)==0);
using _task_t=TaskLambda<_owner_t, _lambda_t>;
UINT size=TypeHelper::AlignUp(sizeof(_task_t), sizeof(SIZE_T))+StackSize;
auto task=(_task_t*)operator new(size);
VOID* stack_end=(VOID*)((SIZE_T)task+size);
new (task) _task_t(Owner, std::forward<_lambda_t>(Lambda), Name, stack_end, StackSize);
Scheduler::AddTask(task);
return task;
}


//==========================
// Con-/Destructors Private
//==========================

inline Handle<Task> Task::CreateInternal(VOID (*Procedure)(), Handle<String> Name, UINT StackSize)
{
assert(StackSize%sizeof(SIZE_T)==0);
UINT size=TypeHelper::AlignUp(sizeof(TaskProcedure), sizeof(SIZE_T))+StackSize;
auto task=(TaskProcedure*)operator new(size);
VOID* stack_end=(VOID*)((SIZE_T)task+size);
new (task) TaskProcedure(Procedure, Name, stack_end, StackSize);
return task;
}

}