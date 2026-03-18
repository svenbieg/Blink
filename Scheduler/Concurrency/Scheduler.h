//=============
// Scheduler.h
//=============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler

#pragma once


//=======
// Using
//=======

#include "Concurrency/CriticalMutex.h"
#include "Concurrency/DispatchedQueue.h"
#include "Concurrency/ReadLock.h"
#include "Concurrency/ServiceTask.h"
#include "Concurrency/Signal.h"
#include "Concurrency/SpinLock.h"
#include "Concurrency/Task.h"
#include "Concurrency/TaskHelper.h"
#include "Concurrency/WriteLock.h"
#include "Devices/System/Cpu.h"


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
	friend CriticalMutex;
	friend Mutex;
	friend Signal;
	friend Task;

	// Con-/Destructors
	Scheduler()=delete;

	// Common
	static VOID Begin();

private:
	// Using
	static const UINT CPU_COUNT=Devices::System::Cpu::CPU_COUNT;
	using CreateList=LINKED_LIST(Task, m_Create);
	using OwnerList=FORWARD_LIST(Task, m_Owners);
	using ReleaseList=LINKED_LIST(Task, m_Release);
	using SleepingList=FORWARD_LIST(Task, m_Sleeping);
	using WaitingList=FORWARD_LIST(Task, m_Waiting);

	// Common
	static VOID AddTask(Task* Task)noexcept;
	static VOID CancelTask(Task* Task)noexcept;
	static UINT CreateTasks()noexcept;
	static VOID ExitTask()noexcept;
	static Task* GetCurrentTask()noexcept;
	static UINT GetNextCore(BOOL Suspend)noexcept;
	static VOID HandleTaskSwitch(VOID* Parameter)noexcept;
	static VOID IdleTask();
	static VOID MainTask();
	static VOID ResumeWaitingTasks(UINT Count, BOOL Suspend)noexcept;
	static VOID Schedule()noexcept;
	static VOID SuspendCurrentTask(UINT MilliSeconds);
	static VOID SuspendCurrentTask(UINT Core, Task* Current, UINT64 ResumeTime=0)noexcept;
	static CreateList s_Create;
	static CriticalSection s_CriticalSection;
	static UINT s_CurrentCore;
	static Task* s_CurrentTask[CPU_COUNT];
	static Task* s_IdleTask[CPU_COUNT];
	static Task* s_MainTask;
	static ReleaseList s_Release;
	static SleepingList s_Sleeping;
	static WaitingList s_Waiting;
};

}