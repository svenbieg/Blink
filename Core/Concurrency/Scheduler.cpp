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
task->m_Owner=current;
s_WaitingTask=AddWaitingTask(s_WaitingTask, task);
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
SuspendCurrentTask(nullptr);
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
	Handle<Task> idle=new Concurrency::Details::TaskProcedure(IdleTask);
	InitializeTask(&idle->m_StackPointer, &Task::TaskProc, idle);
	idle->SetFlag(TaskFlags::Remove);
	s_IdleTask[core]=idle;
	s_CurrentTask[core]=idle;
	}
Handle<Task> main=new Concurrency::Details::TaskProcedure(MainTask);
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
	UINT core=NextCore();
	auto current=s_CurrentTask[core];
	if(GetFlag(current->m_Flags, TaskFlags::Busy))
		continue;
	auto next=GetWaitingTask();
	if(!next)
		break;
	current->SetFlag(TaskFlags::Switch);
	current->m_Next=next;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	}
}

VOID Scheduler::SuspendCurrentTask(UINT ms)
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
current->ClearFlag(TaskFlags::Owner);
current->m_ResumeTime=GetTickCount64()+ms;
if(current->m_Next)
	return;
auto next=GetWaitingTask();
if(!next)
	next=s_IdleTask[core];
current->SetFlag(TaskFlags::Switch);
current->m_Next=next;
Interrupts::Send(IRQ_TASK_SWITCH, core);
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
return first;
}

Handle<Task> Scheduler::AddWaitingTask(Handle<Task> first, Handle<Task> suspend)
{
if(!first)
	return suspend;
auto current_ptr=&first;
while(*current_ptr)
	{
	auto current=*current_ptr;
	assert(current!=suspend);
	if(current->m_ResumeTime>suspend->m_ResumeTime)
		{
		suspend->m_Waiting=current;
		*current_ptr=suspend;
		break;
		}
	if(!current->m_Waiting)
		{
		current->m_Waiting=suspend;
		break;
		}
	current_ptr=&current->m_Waiting;
	}
return first;
}

UINT Scheduler::NextCore()
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
auto current_ptr=&s_WaitingTask;
while(*current_ptr)
	{
	auto current=*current_ptr;
	if(current->m_ResumeTime>time)
		return nullptr;
	if(current->m_Owner)
		{
		auto owner=current->m_Owner;
		if(owner->GetFlag(TaskFlags::Owner))
			{
			current_ptr=&current->m_Waiting;
			continue;
			}
		current->m_Owner=nullptr;
		}
	auto waiting=current->m_Waiting;
	current->m_Waiting=nullptr;
	*current_ptr=waiting;
	return current;
	}
return nullptr;
}

VOID Scheduler::HandleTaskSwitch(VOID* param)
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
ClearFlag(current->m_Flags, TaskFlags::Switch);
auto next=current->m_Next;
current->m_Next=nullptr;
if(!current->GetFlag(TaskFlags::Remove))
	s_WaitingTask=AddWaitingTask(s_WaitingTask, current);
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
auto current_ptr=&first;
while(*current_ptr)
	{
	auto current=*current_ptr;
	if(current==remove)
		{
		auto parallel=current->m_Parallel;
		current->m_Parallel=nullptr;
		parallel->m_Waiting=current->m_Waiting;
		*current_ptr=parallel;
		break;
		}
	current_ptr=&current->m_Parallel;
	}
return first;
}

Handle<Task> Scheduler::RemoveWaitingTask(Handle<Task> first, Handle<Task> remove)
{
auto current_ptr=&first;
while(*current_ptr)
	{
	auto current=*current_ptr;
	if(current==remove)
		{
		auto waiting=current->m_Waiting;
		current->m_Waiting=nullptr;
		*current_ptr=waiting;
		break;
		}
	current_ptr=&current->m_Waiting;
	}
return first;
}

VOID Scheduler::ResumeTask(Handle<Task> resume)
{
for(UINT u=0; u<s_CoreCount; u++)
	{
	if(!resume)
		break;
	UINT core=NextCore();
	auto current=s_CurrentTask[core];
	if(current->GetFlag(TaskFlags::Busy))
		continue;
	resume->ClearFlag(TaskFlags::Remove);
	auto parallel=resume->m_Parallel;
	resume->m_Parallel=nullptr;
	current->SetFlag(TaskFlags::Switch);
	current->m_Next=resume;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	resume=parallel;
	}
while(resume)
	{
	resume->ClearFlag(TaskFlags::Remove);
	auto parallel=resume->m_Parallel;
	resume->m_Parallel=nullptr;
	s_WaitingTask=AddWaitingTask(s_WaitingTask, resume);
	resume=parallel;
	}
}

Handle<Task> Scheduler::SuspendCurrentTask(Handle<Task> owner)
{
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
current->ClearFlag(TaskFlags::Owner);
current->SetFlag(TaskFlags::Remove);
if(!current->m_Next)
	{
	auto next=GetWaitingTask();
	if(!next)
		next=s_IdleTask[core];
	current->SetFlag(TaskFlags::Switch);
	current->m_Next=next;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	}
return AddWaitingTask(owner, current);
}

UINT Scheduler::s_CoreCount=0;
UINT Scheduler::s_CurrentCore=0;
Handle<Task> Scheduler::s_CurrentTask[CPU_COUNT];
Handle<Task> Scheduler::s_IdleTask[CPU_COUNT];
CriticalSection Scheduler::s_CriticalSection;
Handle<Task> Scheduler::s_WaitingTask;

}
