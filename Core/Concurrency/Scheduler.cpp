//===============
// Scheduler.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/System.h"
#include "Devices/System/Tasks.h"
#include "Devices/Timers/SystemTimer.h"
#include "Scheduler.h"

using namespace Devices::System;
using namespace Devices::Timers;

extern INT Main();


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID Scheduler::AddTask(Handle<Task> task)
{
InitializeTask(&task->m_StackPointer, &Task::TaskProc, task);
SpinLock lock(s_CriticalSection);
ResumeTask(task);
}

VOID Scheduler::Begin()
{
SpinLock lock(s_CriticalSection);
UINT core_count=++s_CoreCount;
UINT core=Cpu::GetId();
auto task=s_CurrentTask[core];
lock.Unlock();
Interrupts::Enable();
if(core_count<CPU_COUNT)
	Cpu::PowerOn(core_count);
Cpu::SetContext(&Task::TaskProc, task, task->m_StackPointer);
}

VOID Scheduler::ExitTask()
{
SpinLock lock(s_CriticalSection);
SuspendCurrentTask(nullptr, 0);
lock.Unlock();
while(1)
	{
	Cpu::WaitForInterrupt();
	}
}

Handle<Task> Scheduler::GetCurrentTask()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
return s_CurrentTask[core];
}

VOID Scheduler::Initialize()
{
Interrupts::Route(IRQ_TASK_SWITCH, IrqTarget::All);
Interrupts::SetHandler(IRQ_TASK_SWITCH, HandleTaskSwitch);
for(UINT core=0; core<CPU_COUNT; core++)
	{
	Handle<Task> idle=new Concurrency::Details::TaskTyped(IdleTask);
	InitializeTask(&idle->m_StackPointer, &Task::TaskProc, idle);
	s_IdleTask[core]=idle;
	s_CurrentTask[core]=idle;
	}
Handle<Task> main=new Concurrency::Details::TaskTyped(MainTask);
InitializeTask(&main->m_StackPointer, &Task::TaskProc, main);
s_CurrentTask[0]=main;
}

VOID Scheduler::Schedule()
{
SpinLock lock(s_CriticalSection);
if(!s_WaitingTask)
	return;
for(UINT u=0; u<s_CoreCount; u++)
	{
	UINT core=CurrentCore();
	auto current=s_CurrentTask[core];
	if(GetFlag(current->m_Flags, TaskFlags::Busy))
		continue;
	auto next=GetWaitingTask();
	if(!next)
		break;
	SetFlag(current->m_Flags, TaskFlags::Switch);
	current->m_Next=next;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	}
}

VOID Scheduler::SuspendCurrentTask(UINT ms)
{
SpinLock lock(s_CriticalSection);
s_WaitingTask=SuspendCurrentTask(s_WaitingTask, ms);
}


//================
// Common Private
//================

Task* Scheduler::AddParallelTask(Task* current, Task* parallel)
{
if(!current)
	return parallel;
auto task=current;
while(task->m_Parallel)
	task=task->m_Parallel;
task->m_Parallel=parallel;
return current;
}

UINT Scheduler::CurrentCore()
{
if(++s_CurrentCore==s_CoreCount)
	s_CurrentCore=0;
return s_CurrentCore;
}

Handle<Task> Scheduler::GetWaitingTask()
{
if(!s_WaitingTask)
	return nullptr;
UINT64 time=GetTickCount64();
if(s_WaitingTask->m_ResumeTime>time)
	return nullptr;
auto waiting=s_WaitingTask;
s_WaitingTask=s_WaitingTask->m_Next;
waiting->m_Next=nullptr;
return waiting;
}

VOID Scheduler::HandleTaskSwitch(VOID* param)
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
ClearFlag(current->m_Flags, TaskFlags::Switch);
auto next=current->m_Next;
current->m_Next=nullptr;
BOOL remove=current->GetFlag(TaskFlags::Suspend);
remove|=(current==s_IdleTask[core]);
if(!remove)
	s_WaitingTask=SuspendTask(s_WaitingTask, current, 0);
s_CurrentTask[core]=next;
Cpu::SwitchTask(core, current, next);
}

VOID Scheduler::IdleTask()
{
while(1)
	{
	Cpu::WaitForInterrupt();
	}
}

VOID Scheduler::MainTask()
{
auto timer=SystemTimer::Open();
timer->Tick.Add(Scheduler::Schedule);
Main();
}

Task* Scheduler::RemoveParallelTask(Task* task, Task* remove)
{
if(task==remove)
	{
	auto parallel=task->m_Parallel;
	if(!parallel)
		return nullptr;
	parallel->m_Waiting=task->m_Waiting;
	task->m_Parallel=nullptr;
	task->m_Waiting=nullptr;
	return parallel;
	}
auto current=task;
while(current->m_Parallel)
	{
	if(current->m_Parallel==remove)
		break;
	current=current->m_Parallel;
	}
assert(current->m_Parallel==remove);
current->m_Parallel=remove->m_Parallel;
remove->m_Parallel=nullptr;
return task;
}

VOID Scheduler::ResumeTask(Task* resume)
{
for(UINT u=0; u<s_CoreCount; u++)
	{
	if(!resume)
		break;
	UINT core=CurrentCore();
	auto current=s_CurrentTask[core];
	if(GetFlag(current->m_Flags, TaskFlags::Busy))
		continue;
	SetFlag(current->m_Flags, TaskFlags::Switch);
	current->m_Next=resume;
	auto parallel=resume->m_Parallel;
	resume->m_ResumeTime=0;
	resume->m_Parallel=nullptr;
	resume=parallel;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	}
while(resume)
	{
	s_WaitingTask=SuspendTask(s_WaitingTask, resume, 0);
	auto parallel=resume->m_Parallel;
	resume->m_Parallel=nullptr;
	resume->m_ResumeTime=0;
	resume=parallel;
	}
}

Task* Scheduler::SuspendCurrentTask(Task* owner, UINT ms)
{
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
current->SetFlag(TaskFlags::Suspend);
if(!current->m_Next)
	{
	auto next=GetWaitingTask();
	if(!next)
		next=s_IdleTask[core];
	SetFlag(current->m_Flags, TaskFlags::Switch);
	current->m_Next=next;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	}
return SuspendTask(owner, current, ms);
}

Task* Scheduler::SuspendTask(Task* owner, Task* suspend, UINT ms)
{
suspend->m_ResumeTime=(ms? GetTickCount64()+ms: 0);
if(!owner)
	return suspend;
Task* task=owner;
while(task->m_Waiting)
	{
	auto next=task->m_Waiting;
	if(next->m_ResumeTime>suspend->m_ResumeTime)
		{
		task->m_Waiting=suspend;
		suspend->m_Waiting=next;
		return owner;
		}
	task=next;
	}
task->m_Waiting=suspend;
return owner;
}

UINT Scheduler::s_CoreCount=0;
UINT Scheduler::s_CurrentCore=0;
Handle<Task> Scheduler::s_CurrentTask[CPU_COUNT];
Handle<Task> Scheduler::s_IdleTask[CPU_COUNT];
CriticalSection Scheduler::s_CriticalSection;
Handle<Task> Scheduler::s_WaitingTask;

}
