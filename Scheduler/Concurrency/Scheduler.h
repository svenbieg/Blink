//=============
// Scheduler.h
//=============

#pragma once


//=======
// Using
//=======

#include <config.h>
#include "Concurrency/CriticalSection.h"
#include "Concurrency/Mutex.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class CriticalMutex;
class Signal;
class Task;


//===========
// Scheduler
//===========

class Scheduler
{
public:
	// Friends
	friend CriticalMutex;
	friend Mutex;
	friend Signal;
	friend Task;

	// Common
	static VOID Begin();
	static VOID Initialize();
	static BOOL IsMainTask();

protected:
	// Common
	static VOID AddTask(Task* Task);
	static VOID CancelTask(Task* Task);
	static VOID ExitTask();
	static Task* GetCurrentTask();
	static VOID Schedule();
	static VOID SuspendCurrentTask(UINT MilliSeconds);

private:
	// Common
	static VOID AddCreateTask(Task** First, Task* Task);
	static VOID AddParallelTask(Task** First, Task* Task);
	static VOID AddReleaseTask(Task** First, Task* Task);
	static VOID AddSleepingTask(Task** First, Task* Task);
	static VOID AddWaitingTask(Task* Task, BOOL Prepend=false);
	static BOOL AddWaitingTask(Task** First, Task* Task);
	static BOOL AddWaitingTask(Task** First, Task* Task, AccessMode);
	static VOID CreateTasks();
	static UINT GetAvailableCores(UINT* Cores, UINT Max);
	static UINT GetParallelCount(Task* Task, UINT Max);
	static UINT GetWaitingCount(Task* Task, UINT Max);
	static Task* GetWaitingTask();
	static VOID HandleTaskSwitch(VOID* Parameter);
	static VOID IdleTask();
	static VOID MainTask();
	static BOOL RemoveParallelTask(Task** First, Task* Remove);
	static VOID RemoveSleepingTask(Task** First, Task* Sleeping);
	static VOID ResumeTask(Task* Resume, Status Status=Status::Success);
	static VOID ResumeTask(UINT Core, Task* Current, Task* Resume);
	static VOID SuspendCurrentTask(UINT Core, Task* Current, UINT64 ResumeTime=0);
	static VOID SwitchCurrentTask();
	static VOID SwitchCurrentTask(UINT Core, Task* Current);
	static UINT s_CoreCount;
	static Task* s_Create;
	static CriticalSection s_CriticalSection;
	static Task* s_CurrentTask[CPU_COUNT];
	static Task* s_IdleTask[CPU_COUNT];
	static Task* s_MainTask;
	static Task* s_Release;
	static Task* s_Sleeping;
	static Task* s_WaitingFirst;
	static Task* s_WaitingLast;
	static Task* s_WaitingLocked;
};

}