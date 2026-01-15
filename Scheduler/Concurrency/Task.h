//========
// Task.h
//========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#task-creation

#pragma once


//=======
// Using
//=======

#include "Collections/ForwardList.h"
#include "Collections/LinkedList.h"
#include "Concurrency/DispatchedHandler.h"
#include "Concurrency/Mutex.h"
#include "MemoryHelper.h"
#include "StringClass.h"
#include "UnwindException.h"
#include <assert.h>
#include <new>
#include <utility>


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class CriticalMutex;
class Scheduler;
class ServiceTask;
class Signal;
class TaskHelper;


//=======
// Flags
//=======

enum class TaskFlags: UINT
{
None=0,
Done=(1<<0),
Idle=(1<<1),
Priority=(1<<2),
PrioritySharing=(1<<2)|(1<<5),
Creator=(1<<3),
Release=(1<<4),
Sharing=(1<<5),
Suspended=(1<<7),
Timeout=(1<<8)
};


//======
// Task
//======

class Task: public Object
{
public:
	// Friends
	friend CriticalMutex;
	friend Mutex;
	friend Scheduler;
	friend ServiceTask;
	friend Signal;
	friend TaskHelper;
	friend UnwindException;

	// Con-/Destructors
	~Task();
	static Handle<Task> Create(VOID (*Procedure)(), Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE);
	template <class _owner_t> static Handle<Task> Create(_owner_t* Owner, VOID (_owner_t::*Procedure)(), Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE);
	template <class _owner_t> static inline Handle<Task> Create(Handle<_owner_t> const& Owner, VOID (_owner_t::*Procedure)(), Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE)
		{
		return Create((_owner_t*)Owner, Procedure, Name, StackSize);
		}
	template <class _owner_t, class _lambda_t> static Handle<Task> Create(_owner_t* Owner, _lambda_t&& Lambda, Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE);
	template <class _owner_t, class _lambda_t> static inline Handle<Task> Create(Handle<_owner_t> const& Owner, _lambda_t&& Lambda, Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE)
		{
		return Create((_owner_t*)Owner, std::forward<_lambda_t>(Lambda), Name, StackSize);
		}
	template <class _lambda_t> static inline Handle<Task> Create(nullptr_t Owner, _lambda_t&& Lambda, Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE);

	// Common
	VOID Cancel();
	volatile BOOL Cancelled;
	static Handle<Task> Get();
	inline Status GetStatus()const { return m_Status; }
	static BOOL IsMainTask();
	const LPCTSTR Name;
	Handle<Object> Result;
	static VOID Sleep(UINT Milliseconds);
	inline VOID Then(VOID (*Procedure)())
		{
		assert(m_Then==nullptr);
		m_Then=new DispatchedProcedure(Procedure);
		}
	template <class _owner_t> inline VOID Then(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		assert(m_Then==nullptr);
		m_Then=new DispatchedMemberProcedure<_owner_t>(Owner, Procedure);
		}
	template <class _owner_t, class... _args_t> inline VOID Then(Handle<_owner_t> const& Owner, VOID (_owner_t::*Procedure)())
		{
		assert(m_Then==nullptr);
		m_Then=new DispatchedMemberProcedure<_owner_t>(Owner, Procedure);
		}
	template <class _owner_t, class _lambda_t> inline VOID Then(_owner_t* Owner, _lambda_t&& Lambda)
		{
		assert(m_Then==nullptr);
		m_Then=new DispatchedLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
		}
	template <class _owner_t, class _lambda_t> inline VOID Then(Handle<_owner_t> const& Owner, _lambda_t&& Lambda)
		{
		assert(m_Then==nullptr);
		m_Then=new DispatchedLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
		}
	template <class _lambda_t> inline VOID Then(nullptr_t Owner, _lambda_t&& Lambda)
		{
		assert(m_Then==nullptr);
		m_Then=new DispatchedLambda<nullptr_t, _lambda_t>(std::forward<_lambda_t>(Lambda));
		}

protected:
	// Using
	template <class _item_t> using FwdLink=Collections::FwdLink<_item_t>;
	template <class _item_t> using Link=Collections::Link<_item_t>;

	// Con-/Destructors
	Task(BYTE* Stack, SIZE_T StackSize, Handle<String> Name);
	static Task* CreateInternal(VOID (*Procedure)(), Handle<String> Name, SIZE_T StackSize=MemoryHelper::PAGE_SIZE);

	// Common
	static bool Priority(Task* First, Task* Second);
	virtual VOID Run()=0;
	static VOID Schedule(Task* Task);
	static VOID TaskProc(VOID* Parameter);
	Link<Task> m_Create;
	Task* m_Creator;
	UnwindException* m_Exception;
	TaskFlags m_Flags;
	Mutex m_Mutex;
	Handle<String> m_Name;
	Task* m_Next;
	FwdLink<Task> m_Owners;
	UINT m_PriorityCount;
	Link<Task> m_Release;
	UINT64 m_ResumeTime;
	Signal* m_Signal;
	UINT m_SignalCount;
	Link<Task> m_Sleeping;
	SIZE_T m_StackBottom;
	SIZE_T m_StackPointer;
	SIZE_T m_StackSize;
	Status m_Status;
	DispatchedHandler* m_Then;
	Handle<Task> m_This;
	FwdLink<Task> m_Waiting;
};


//===========
// Procedure
//===========

class TaskProcedure: public Task
{
private:
	// Using
	typedef VOID (*_proc_t)();

	// Con-/Destructors
	friend Task;
	TaskProcedure(BYTE* Stack, SIZE_T StackSize, _proc_t Procedure, Handle<String> Name):
		Task(Stack, StackSize, Name),
		m_Procedure(Procedure)
		{}

	// Common
	VOID Run()override { m_Procedure(); }
	_proc_t m_Procedure;
};


//==================
// Member-Procedure
//==================

template <class _owner_t> class TaskMemberProcedure: public Task
{
private:
	// Using
	typedef VOID (_owner_t::*_proc_t)();

	// Con-/Destructors
	friend Task;
	TaskMemberProcedure(BYTE* Stack, SIZE_T StackSize, _owner_t* Owner, _proc_t Procedure, Handle<String> Name):
		Task(Stack, StackSize, Name),
		m_Owner(Owner),
		m_Procedure(Procedure)
		{}

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
private:
	// Con-/Destructors
	friend Task;
	TaskLambda(BYTE* Stack, SIZE_T StackSize, _owner_t* Owner, _lambda_t&& Lambda, Handle<String> Name):
		Task(Stack, StackSize, Name),
		m_Lambda(std::move(Lambda)),
		m_Owner(Owner)
		{}

	// Common
	VOID Run()override { m_Lambda(); }
	_lambda_t m_Lambda;
	Handle<_owner_t> m_Owner;
};

template <class _lambda_t> class TaskLambda<nullptr_t, _lambda_t>: public Task
{
private:
	// Con-/Destructors
	friend Task;
	TaskLambda(BYTE* Stack, SIZE_T StackSize, nullptr_t Owner, _lambda_t&& Lambda, Handle<String> Name):
		Task(Stack, StackSize, Name),
		m_Lambda(std::move(Lambda))
		{}

	// Common
	VOID Run()override { m_Lambda(); }
	_lambda_t m_Lambda;
};


//==================
// Con-/Destructors
//==================

template <class _owner_t> Handle<Task> Task::Create(_owner_t* Owner, VOID (_owner_t::*Procedure)(), Handle<String> Name, SIZE_T StackSize)
{
assert(StackSize%sizeof(SIZE_T)==0);
using task_t=TaskMemberProcedure<_owner_t>;
SIZE_T task_size=TypeHelper::AlignUp(sizeof(task_t), sizeof(SIZE_T));
auto task=(task_t*)MemoryHelper::Allocate(task_size+StackSize);
auto stack=(BYTE*)task+task_size;
new (task) task_t(stack, StackSize, Owner, Procedure, Name);
Schedule(task);
return task;
}

template <class _owner_t, class _lambda_t> Handle<Task> Task::Create(_owner_t* Owner, _lambda_t&& Lambda, Handle<String> Name, SIZE_T StackSize)
{
assert(StackSize%sizeof(SIZE_T)==0);
using task_t=TaskLambda<_owner_t, _lambda_t>;
SIZE_T task_size=TypeHelper::AlignUp(sizeof(task_t), sizeof(SIZE_T));
auto task=(task_t*)MemoryHelper::Allocate(task_size+StackSize);
auto stack=(BYTE*)task+task_size;
new (task) task_t(stack, StackSize, Owner, std::forward<_lambda_t>(Lambda), Name);
Schedule(task);
return task;
}

template <class _lambda_t> Handle<Task> Task::Create(nullptr_t Owner, _lambda_t&& Lambda, Handle<String> Name, SIZE_T StackSize)
{
assert(StackSize%sizeof(SIZE_T)==0);
using task_t=TaskLambda<nullptr_t, _lambda_t>;
SIZE_T task_size=TypeHelper::AlignUp(sizeof(task_t), sizeof(SIZE_T));
auto task=(task_t*)MemoryHelper::Allocate(task_size+StackSize);
auto stack=(BYTE*)task+task_size;
new (task) task_t(stack, StackSize, nullptr, std::forward<_lambda_t>(Lambda), Name);
Schedule(task);
return task;
}

}