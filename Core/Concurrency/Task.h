//========
// Task.h
//========

#pragma once


//=======
// Using
//=======

#include "Core/DispatchedHandler.h"
#include "Devices/System/Cpu.h"
#include "Signal.h"


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
Suspend=16
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
	template <class _owner_t> inline VOID Then(VOID (*Procedure)())
		{
		auto handler=new Core::Details::DispatchedProcedure(Procedure);
		DispatchedHandler::Append(m_Then, handler);
		}
	template <class _owner_t> inline VOID Then(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		auto handler=new Core::Details::DispatchedMemberFunction<_owner_t>(Owner, [Owner, Procedure]() { (Owner->*Procedure)(); });
		DispatchedHandler::Append(m_Then, handler);
		}
	inline VOID Then(Function<VOID()> Function)
		{
		auto handler=new Core::Details::DispatchedFunction<Task>(this, Function);
		DispatchedHandler::Append(m_Then, handler);
		}
	inline Status GetStatus()const { return m_Status; }
	Status Wait();

protected:
	// Con-/Destructors
	Task();

	// Common
	Status m_Status;

private:
	// Using
	using DispatchedHandler=Core::DispatchedHandler;

	// Common
	inline VOID ClearFlag(TaskFlags Flag) { ::ClearFlag(m_Flags, Flag); }
	inline BOOL GetFlag(TaskFlags Flag)const { return ::GetFlag(m_Flags, Flag); }
	inline VOID SetFlag(TaskFlags Flag) { ::SetFlag(m_Flags, Flag); }
	virtual VOID Run()=0;
	static VOID TaskProc(VOID* Parameter);
	Signal m_Done;
	TaskFlags m_Flags;
	Mutex m_Mutex;
	Handle<Task> m_Next;
	Handle<Task> m_Owner;
	Handle<Task> m_Parallel;
	UINT64 m_ResumeTime;
	VOID* m_StackPointer;
	Handle<DispatchedHandler> m_Then;
	Handle<Task> m_Waiting;
	ALIGN(sizeof(SIZE_T)) BYTE m_Stack[STACK_SIZE];
};


//============
// Task-Typed
//============

namespace Details {
template <class... _args_t> class TaskTyped: public Task
{
public:
	// Using
	typedef VOID (*_proc_t)(_args_t...);

	// Con-/Destructors
	TaskTyped(_proc_t Procedure, _args_t... Arguments):
		m_Function([Procedure, Arguments...]() { (*Procedure)(Arguments...); })
		{}

private:
	// Common
	VOID Run()override { m_Function(); }
	Function<VOID()> m_Function;
};}


//============
// Task-Owned
//============

namespace Details {
template <class _owner_t, class... _args_t> class TaskOwned: public Task
{
public:
	// Using
	typedef VOID (_owner_t::*_proc_t)(_args_t...);

	// Con-/Destructors
	TaskOwned(_owner_t* Owner, _proc_t Procedure, _args_t... Arguments):
		m_Function([this, Owner, Procedure, Arguments...]() { (Owner->*Procedure)(Arguments...); }),
		m_Owner(Owner)
		{}

private:
	// Common
	VOID Run()override { m_Function(); }
	Function<VOID()> m_Function;
	Handle<_owner_t> m_Owner;
};}

}
