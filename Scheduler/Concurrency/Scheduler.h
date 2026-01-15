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
#include "Concurrency/Signal.h"
#include "Concurrency/SpinLock.h"
#include "Concurrency/Task.h"
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
	static constexpr UINT CPU_COUNT=Devices::System::Cpu::CPU_COUNT;
	using CreateList=Collections::LinkedList<LNK_LIST(Task, m_Create)>;
	using OwnerList=Collections::ForwardList<FWD_LIST(Task, m_Owners)>;
	using ReleaseList=Collections::LinkedList<LNK_LIST(Task, m_Release)>;
	using SleepingList=Collections::ForwardList<FWD_LIST(Task, m_Sleeping)>;
	using WaitingList=Collections::ForwardList<FWD_LIST(Task, m_Waiting)>;

	// Common
	static VOID AddTask(Task* Task);
	static VOID CancelTask(Task* Task);
	static UINT CreateTasks();
	static VOID ExitTask();
	static Task* GetCurrentTask();
	static UINT GetNextCore(BOOL Suspend);
	static VOID HandleTaskSwitch(VOID* Parameter);
	static VOID IdleTask();
	static VOID MainTask();
	static VOID ResumeWaitingTasks(UINT Count, BOOL Suspend);
	static VOID Schedule();
	static VOID SuspendCurrentTask(UINT MilliSeconds);
	static VOID SuspendCurrentTask(UINT Core, Task* Current, UINT64 ResumeTime=0);
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