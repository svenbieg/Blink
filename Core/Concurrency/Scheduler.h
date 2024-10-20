//=============
// Scheduler.h
//=============

#pragma once


//=======
// Using
//=======

#include "Settings.h"
#include "SpinLock.h"
#include "Task.h"


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
	friend class Mutex;
	friend class Signal;

	// Common
	static VOID AddTask(Handle<Task> Task);
	static VOID Begin();
	static VOID ExitTask();
	static Handle<Task> GetCurrentTask();
	static VOID Initialize();
	static VOID Schedule();
	static VOID SuspendCurrentTask(UINT MilliSeconds);

private:
	// Common
	static Handle<Task> AddParallelTask(Handle<Task> First, Handle<Task> Parallel);
	static Handle<Task> AddWaitingTask(Handle<Task> First, Handle<Task> Suspend, UINT MilliSeconds);
	static UINT CurrentCore();
	static Handle<Task> GetWaitingTask();
	static VOID HandleTaskSwitch(VOID* Parameter);
	static VOID IdleTask();
	static VOID MainTask();
	static Handle<Task> RemoveParallelTask(Handle<Task> First, Handle<Task> Remove);
	static Handle<Task> RemoveWaitingTask(Handle<Task> First, Handle<Task> Remove);
	static VOID ResumeTask(Handle<Task> Resume);
	static Handle<Task> SuspendCurrentTask(Handle<Task> Owner, UINT MilliSeconds);
	static UINT s_CoreCount;
	static UINT s_CurrentCore;
	static Handle<Task> s_CurrentTask[CPU_COUNT];
	static Handle<Task> s_IdleTask[CPU_COUNT];
	static CriticalSection s_CriticalSection;
	static Handle<Task> s_WaitingTask;
};

}
