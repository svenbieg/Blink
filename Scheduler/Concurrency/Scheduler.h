//=============
// Scheduler.h
//=============

#pragma once


//=======
// Using
//=======

#include <config.h>
#include "Concurrency/CriticalSection.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class Mutex;
class Signal;
class Task;


//===========
// Scheduler
//===========

class Scheduler
{
public:
	// Friends
	friend Mutex;
	friend Signal;
	friend Task;

	// Common
	static VOID Begin();
	static VOID Initialize();
	static BOOL IsMainTask();

private:
	// Common
	static Handle<Task> AddParallelTask(Handle<Task> First, Handle<Task> Parallel);
	static VOID AddTask(Task* Task);
	static Handle<Task> AddWaitingTask(Handle<Task> First, Handle<Task> Suspend);
	static VOID ExitTask();
	static Handle<Task> GetCurrentTask();
	static Handle<Task> GetWaitingTask();
	static VOID HandleTaskSwitch(VOID* Parameter);
	static VOID IdleTask();
	static VOID MainTask();
	static UINT NextCore();
	static Handle<Task> RemoveParallelTask(Handle<Task> First, Handle<Task> Remove);
	static Handle<Task> RemoveWaitingTask(Handle<Task> First, Handle<Task> Remove);
	static VOID ResumeTask(Handle<Task> Resume);
	static VOID Schedule();
	static VOID SuspendCurrentTask(UINT MilliSeconds);
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