//========
// Task.h
//========

#pragma once


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"
#include "Devices/System/Cpu.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class Mutex;
class Scheduler;
class Signal;


//=======
// Flags
//=======

enum class TaskFlags: UINT
{
None=0,
Blocking=1,
Switch=2,
Owner=4,
Busy=7,
Sharing=8,
Remove=16
};


//======
// Task
//======

class Task: public Object
{
public:
	// Friends
	friend Devices::System::Cpu;
	friend Mutex;
	friend Scheduler;
	friend Signal;

	// Common
	VOID Cancel();
	volatile BOOL Cancelled;
	static Handle<Task> Get();
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
	template <class _owner_t, class... _args_t> inline VOID Then(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)())
		{
		assert(m_Then==nullptr);
		m_Then=new DispatchedMemberProcedure<_owner_t>(Owner, Procedure);
		}
	template <class _owner_t, class _lambda_t> inline VOID Then(_owner_t* Owner, _lambda_t&& Lambda)
		{
		assert(m_Then==nullptr);
		m_Then=new DispatchedLambda(Owner, std::forward<_lambda_t>(Lambda));
		}
	inline Status GetStatus()const { return m_Status; }
	static VOID Sleep(UINT Milliseconds);
	static VOID SleepMicroseconds(UINT Microseconds);
	Status Wait();

protected:
	// Con-/Destructors
	Task();

	// Common
	Status m_Status;

private:
	// Common
	inline VOID ClearFlag(TaskFlags Flag) { FlagHelper::Clear(m_Flags, Flag); }
	inline BOOL GetFlag(TaskFlags Flag)const { return FlagHelper::Get(m_Flags, Flag); }
	inline VOID SetFlag(TaskFlags Flag) { FlagHelper::Set(m_Flags, Flag); }
	virtual VOID Run()=0;
	static VOID TaskProc(VOID* Parameter);
	UINT m_BlockingCount;
	Signal m_Done;
	TaskFlags m_Flags;
	Mutex m_Mutex;
	Handle<Task> m_Next;
	Handle<Task> m_Owner;
	Handle<Task> m_Parallel;
	UINT64 m_ResumeTime;
	VOID* m_StackPointer;
	DispatchedHandler* m_Then;
	Handle<Task> m_Waiting;
	ALIGN(sizeof(SIZE_T)) BYTE m_Stack[STACK_SIZE];
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
	TaskProcedure(_proc_t Procedure):
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
	TaskMemberProcedure(_owner_t* Owner, _proc_t Procedure):
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
	TaskLambda(_owner_t* Owner, _lambda_t&& Lambda):
		m_Lambda(std::move(Lambda)),
		m_Owner(Owner)
		{}

private:
	// Common
	VOID Run()override { m_Lambda(); }
	_lambda_t m_Lambda;
	Handle<_owner_t> m_Owner;
};

}