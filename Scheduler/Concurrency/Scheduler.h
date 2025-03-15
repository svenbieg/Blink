//=============
// Scheduler.h
//=============

#pragma once


//=======
// Using
//=======

#include <config.h>
#include "Collections/map.hpp"
#include "Concurrency/CriticalSection.h"
#include "Concurrency/Mutex.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

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
	static VOID AddParallelTask(Handle<Task>* First, Task* Parallel);
	static VOID AddSleepingTask(Task* Task, UINT64 ResumeTime);
	static VOID AddSleepingTask(Handle<Task>* First, Task* Sleeping);
	static VOID AddTask(Task* Task);
	static VOID AddWaitingTask(Handle<Task>* First, Task* Waiting);
	static VOID AddWakeupTask(Handle<Task>* First, Task* Wakeup);
	static VOID CancelTask(Task* Task);
	static VOID ExitTask();
	static UINT GetCurrentCore();
	static Handle<Task> GetCurrentTask();
	static BOOL GetNextCore(UINT* Core);
	static Handle<Task> GetWaitingTask();
	static VOID HandleTaskSwitch(VOID* Parameter);
	static VOID IdleTask();
	static VOID MainTask();
	static VOID RemoveParallelTask(Handle<Task>* First, Task* Remove);
	static VOID RemoveSleepingTask(Handle<Task>* First, Task* Sleeping);
	static VOID ResumeTask(Task* Resume, Status Status=Status::Success);
	static VOID Schedule();
	static VOID SuspendCurrentTask(UINT MilliSeconds);
	static VOID SuspendCurrentTask(Handle<Task>* Owner);
	static Handle<Task> WakeupTasks();
	static UINT s_CoreCount;
	static CriticalSection s_CriticalSection;
	static UINT s_CurrentCore;
	static Handle<Task> s_CurrentTask[CPU_COUNT];
	static Handle<Task> s_IdleTask[CPU_COUNT];
	static Task* s_MainTask;
	static Mutex s_SleepingMutex;
	static Collections::map<UINT64, Handle<Task>> s_SleepingTasks;
	static Handle<Task> s_WaitingTask;
	static Handle<Task> s_WakeupTask;
};

}