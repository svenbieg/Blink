//===============
// Scheduler.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Concurrency/SpinLock.h"
#include "Concurrency/Task.h"
#include "Concurrency/TaskHelper.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/System.h"
#include "Devices/Timers/SystemTimer.h"

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

VOID Scheduler::Begin()
{
SpinLock lock(s_CriticalSection);
UINT core_count=++s_CoreCount;
UINT core=Cpu::GetId();
auto task=s_CurrentTask[core];
lock.Unlock();
Interrupts::Enable();
Cpu::SetContext(&Task::TaskProc, task, task->m_StackPointer);
}

VOID Scheduler::Initialize()
{
Interrupts::Route(IRQ_TASK_SWITCH, IrqTarget::All);
Interrupts::SetHandler(IRQ_TASK_SWITCH, HandleTaskSwitch);
for(UINT core=0; core<CPU_COUNT; core++)
	{
	auto idle=Task::CreateInternal(IdleTask, String::Create("idle%u", core));
	idle->SetFlag(TaskFlags::Remove);
	s_IdleTask[core]=idle;
	s_CurrentTask[core]=idle;
	}
auto main=Task::CreateInternal(MainTask, "main");
s_CurrentTask[0]=main;
s_MainTask=main;
}

BOOL Scheduler::IsMainTask()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
return current==s_MainTask;
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

VOID Scheduler::AddTask(Task* task)
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
current->SetFlag(TaskFlags::Owner);
task->m_Owner=current;
s_WaitingTask=AddWaitingTask(s_WaitingTask, task);
}

Handle<Task> Scheduler::AddWaitingTask(Handle<Task> first, Handle<Task> suspend)
{
if(!first)
	return suspend;
auto current_ptr=&first;
if(suspend->GetFlag(TaskFlags::Locked))
	{
	assert(suspend->m_ResumeTime==0);
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=suspend);
		if(!current->GetFlag(TaskFlags::Locked))
			{
			suspend->m_Waiting=current;
			*current_ptr=suspend;
			return first;
			}
		if(!current->m_Waiting)
			{
			current->m_Waiting=suspend;
			return first;
			}
		current_ptr=&current->m_Waiting;
		}
	assert(0);
	}
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

Handle<Task> Scheduler::GetWaitingTask()
{
if(!s_WaitingTask)
	return nullptr;
UINT64 time=SystemTimer::GetTickCount64();
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
FlagHelper::Clear(current->m_Flags, TaskFlags::Switch);
auto next=current->m_Next;
current->m_Next=nullptr;
if(!current->GetFlag(TaskFlags::Remove))
	s_WaitingTask=AddWaitingTask(s_WaitingTask, current);
s_CurrentTask[core]=next;
TaskHelper::Switch(core, current, next);
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
auto timer=SystemTimer::Get();
timer->Triggered.Add(Scheduler::Schedule);
try
	{
	Main();
	}
catch(...) {}
System::Restart();
}

UINT Scheduler::NextCore()
{
if(++s_CurrentCore==s_CoreCount)
	s_CurrentCore=0;
return s_CurrentCore;
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

VOID Scheduler::Schedule()
{
SpinLock lock(s_CriticalSection);
if(!s_WaitingTask)
	return;
for(UINT u=0; u<s_CoreCount; u++)
	{
	UINT core=NextCore();
	auto current=s_CurrentTask[core];
	if(FlagHelper::Get(current->m_Flags, TaskFlags::Busy))
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
assert(current!=s_MainTask);
assert(current->m_Next==nullptr);
current->ClearFlag(TaskFlags::Owner);
current->m_ResumeTime=SystemTimer::GetTickCount64()+ms;
auto next=GetWaitingTask();
if(!next)
	next=s_IdleTask[core];
current->SetFlag(TaskFlags::Switch);
current->m_Next=next;
Interrupts::Send(IRQ_TASK_SWITCH, core);
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
CriticalSection Scheduler::s_CriticalSection;
UINT Scheduler::s_CurrentCore=0;
Handle<Task> Scheduler::s_CurrentTask[CPU_COUNT];
Handle<Task> Scheduler::s_IdleTask[CPU_COUNT];
Task* Scheduler::s_MainTask=nullptr;
Handle<Task> Scheduler::s_WaitingTask;

}