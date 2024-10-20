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
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
current->SetFlag(TaskFlags::Owner);
s_WaitingTask=AddWaitingTask(s_WaitingTask, task, 0);
}

VOID Scheduler::Begin()
{
SpinLock lock(s_CriticalSection);
UINT core_count=++s_CoreCount;
UINT core=Cpu::GetId();
auto task=s_CurrentTask[core];
lock.Unlock();
if(core_count<CPU_COUNT)
	Cpu::PowerOn(core_count);
Interrupts::Enable();
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
		{
		current->ClearFlag(TaskFlags::Owner);
		continue;
		}
	auto next=GetWaitingTask();
	if(!next)
		break;
	if(current!=s_IdleTask[core])
		current->SetFlag(TaskFlags::Suspend);
	current->SetFlag(TaskFlags::Switch);
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

Handle<Task> Scheduler::AddParallelTask(Handle<Task> first, Handle<Task> parallel)
{
if(!first)
	return parallel;
auto current=first;
while(current->m_Parallel)
	current=current->m_Parallel;
current->m_Parallel=parallel;
return current;
}

Handle<Task> Scheduler::AddWaitingTask(Handle<Task> first, Handle<Task> suspend, UINT ms)
{
suspend->m_ResumeTime=(ms? GetTickCount64()+ms: 0);
if(!first)
	return suspend;
Handle<Task> current=first;
while(current->m_Waiting)
	{
	auto waiting=current->m_Waiting;
	if(waiting->m_ResumeTime>suspend->m_ResumeTime)
		{
		suspend->m_Waiting=waiting;
		current->m_Waiting=suspend;
		return first;
		}
	current=waiting;
	}
current->m_Waiting=suspend;
return first;
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
s_WaitingTask=s_WaitingTask->m_Waiting;
waiting->m_Waiting=nullptr;
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
if(current->GetFlag(TaskFlags::Suspend))
	{
	current->ClearFlag(TaskFlags::Suspend);
	s_WaitingTask=AddWaitingTask(s_WaitingTask, current, 0);
	}
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

Handle<Task> Scheduler::RemoveParallelTask(Handle<Task> first, Handle<Task> remove)
{
if(first==remove)
	{
	auto parallel=first->m_Parallel;
	if(!parallel)
		return nullptr;
	parallel->m_Waiting=first->m_Waiting;
	first->m_Parallel=nullptr;
	first->m_Waiting=nullptr;
	return parallel;
	}
auto current=first;
while(current->m_Parallel)
	{
	if(current->m_Parallel==remove)
		break;
	current=current->m_Parallel;
	}
assert(current->m_Parallel==remove);
current->m_Parallel=remove->m_Parallel;
remove->m_Parallel=nullptr;
return first;
}

Handle<Task> Scheduler::RemoveWaitingTask(Handle<Task> first, Handle<Task> remove)
{
if(!first)
	return nullptr;
if(first==remove)
	{
	auto waiting=first->m_Waiting;
	first->m_Waiting=nullptr;
	return waiting;
	}
auto current=first;
while(current->m_Waiting)
	{
	auto waiting=current->m_Waiting;
	if(waiting==remove)
		{
		current->m_Waiting=remove->m_Waiting;
		remove->m_Waiting=nullptr;
		return first;
		}
	current=waiting;
	}
return first;
}

VOID Scheduler::ResumeTask(Handle<Task> resume)
{
for(UINT u=0; u<s_CoreCount; u++)
	{
	if(!resume)
		break;
	UINT core=CurrentCore();
	auto current=s_CurrentTask[core];
	if(GetFlag(current->m_Flags, TaskFlags::Busy))
		continue;
	s_WaitingTask=RemoveWaitingTask(s_WaitingTask, resume);
	auto parallel=resume->m_Parallel;
	resume->m_Parallel=nullptr;
	resume->m_ResumeTime=0;
	if(current!=s_IdleTask[core])
		current->SetFlag(TaskFlags::Suspend);
	current->SetFlag(TaskFlags::Switch);
	current->m_Next=resume;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	resume=parallel;
	}
while(resume)
	{
	s_WaitingTask=RemoveWaitingTask(s_WaitingTask, resume);
	auto parallel=resume->m_Parallel;
	resume->m_Parallel=nullptr;
	resume->m_ResumeTime=0;
	s_WaitingTask=AddWaitingTask(s_WaitingTask, resume, 0);
	resume=parallel;
	}
}

Handle<Task> Scheduler::SuspendCurrentTask(Handle<Task> owner, UINT ms)
{
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
current->ClearFlag(TaskFlags::Owner);
if(!current->m_Next)
	{
	auto next=GetWaitingTask();
	if(!next)
		next=s_IdleTask[core];
	current->SetFlag(TaskFlags::Switch);
	current->m_Next=next;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	}
return AddWaitingTask(owner, current, ms);
}

UINT Scheduler::s_CoreCount=0;
UINT Scheduler::s_CurrentCore=0;
Handle<Task> Scheduler::s_CurrentTask[CPU_COUNT];
Handle<Task> Scheduler::s_IdleTask[CPU_COUNT];
CriticalSection Scheduler::s_CriticalSection;
Handle<Task> Scheduler::s_WaitingTask;

}
