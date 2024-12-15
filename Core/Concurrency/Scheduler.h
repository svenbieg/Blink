//=============
// Scheduler.h
//=============

#pragma once


//=======
// Using
//=======

#include "Settings.h"
#include "Signal.h"
#include "SharedLock.h"
#include "SpinLock.h"
#include "Task.h"
#include "TaskLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//===========
// Scheduler
//===========

class Scheduler
{
public:
	// Friends
	friend Mutex;
	friend Signal;

	// Common
	static VOID AddTask(Task* Task);
	static VOID Begin();
	template <class _owner_t, class... _args_t>
	static inline Handle<Task> CreateTask(VOID (*Procedure)())
		{
		auto task=new TaskProcedure(Procedure);
		AddTask(task);
		return task;
		}
	template <class _owner_t>
	static inline Handle<Task> CreateTask(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		auto task=new TaskMemberProcedure(Owner, Procedure);
		AddTask(task);
		return task;
		}
	template <class _owner_t>
	static inline Handle<Task> CreateTask(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)())
		{
		auto task=new TaskMemberProcedure(Owner, Procedure);
		AddTask(task);
		return task;
		}
	template <class _owner_t, class _lambda_t>
	static inline Handle<Task> CreateTask(_owner_t* Owner, _lambda_t&& Lambda)
		{
		auto task=new TaskLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
		AddTask(task);
		return task;
		}
	template <class _owner_t, class _lambda_t>
	static inline Handle<Task> CreateTask(Handle<_owner_t> Owner, _lambda_t&& Lambda)
		{
		auto task=new TaskLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
		AddTask(task);
		return task;
		}
	static VOID ExitTask();
	static Handle<Task> GetCurrentTask();
	static VOID Initialize();
	static BOOL IsMainTask();
	static VOID Schedule();
	static VOID SuspendCurrentTask(UINT MilliSeconds);

private:
	// Common
	static Handle<Task> AddParallelTask(Handle<Task> First, Handle<Task> Parallel);
	static Handle<Task> AddWaitingTask(Handle<Task> First, Handle<Task> Suspend);
	static Handle<Task> GetWaitingTask();
	static VOID HandleTaskSwitch(VOID* Parameter);
	static VOID IdleTask();
	static VOID MainTask();
	static UINT NextCore();
	static Handle<Task> RemoveParallelTask(Handle<Task> First, Handle<Task> Remove);
	static Handle<Task> RemoveWaitingTask(Handle<Task> First, Handle<Task> Remove);
	static VOID ResumeTask(Handle<Task> Resume);
	static Handle<Task> SuspendCurrentTask(Handle<Task> Owner);
	static UINT s_CoreCount;
	static CriticalSection s_CriticalSection;
	static UINT s_CurrentCore;
	static Handle<Task> s_CurrentTask[CPU_COUNT];
	static Handle<Task> s_IdleTask[CPU_COUNT];
	static Task* s_MainTask;
	static Handle<Task> s_WaitingTask;
};

}
