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
for(UINT core=0; core<CPU_COUNT; core++)
	{
	if(s_CurrentTask[core]!=s_IdleTask[core])
		continue;
	if(s_CurrentTask[core]->m_Next)
		continue;
	s_CurrentTask[core]->m_Next=task;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	return;
	}
s_WaitingTask=SuspendTask(s_WaitingTask, task, 0);
}

VOID Scheduler::Begin()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto task=s_CurrentTask[core];
lock.Unlock();
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
if(CPU_COUNT>0)
	s_CurrentCore=1;
for(UINT core=0; core<CPU_COUNT; core++)
	{
	Handle<Task> idle_task=new Concurrency::Details::TaskTyped(IdleTask);
	s_IdleTask[core]=idle_task;
	s_CurrentTask[core]=idle_task;
	}
s_WaitingTask=new Concurrency::Details::TaskTyped(MainTask);
InitializeTask(&s_WaitingTask->m_StackPointer, &Task::TaskProc, s_WaitingTask);
Interrupts::Route(IRQ_TASK_SWITCH, IrqTarget::All);
Interrupts::SetHandler(IRQ_TASK_SWITCH, HandleTaskSwitch);
auto timer=SystemTimer::Open();
timer->Tick.Add(&Scheduler::OnSystemTimerTick);
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
auto next=current->m_Next;
if(!current->GetFlag(TaskFlags::Suspend))
	{
	if(current!=s_IdleTask[core])
		s_WaitingTask=SuspendTask(s_WaitingTask, current, 0);
	}
if(next->GetFlag(TaskFlags::Exclusive))
	Interrupts::Disable();
s_CurrentTask[core]=next;
Cpu::SwitchTask(core, current, next);
}

VOID Scheduler::IdleTask()
{
Interrupts::Enable();
while(1)
	{
	Cpu::WaitForInterrupt();
	}
}

VOID Scheduler::MainTask()
{
Main();
}

VOID Scheduler::OnSystemTimerTick()
{
SpinLock lock(s_CriticalSection);
if(!s_WaitingTask)
	return;
for(UINT core=0; core<CPU_COUNT; core++)
	{
	auto current=s_CurrentTask[s_CurrentCore];
	if(!current->m_Next)
		{
		auto next=GetWaitingTask();
		if(!next)
			break;
		current->m_Next=next;
		if(core==0)
			{
			if(next->GetFlag(TaskFlags::Exclusive))
				Interrupts::Disable();
			s_CurrentTask[core]=next;
			Cpu::SwitchTask(core, current, next);
			}
		else
			{
			Interrupts::Send(IRQ_TASK_SWITCH, s_CurrentCore);
			}
		}
	if(++s_CurrentCore==CPU_COUNT)
		s_CurrentCore=0;
	}
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
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
assert(!current->m_Next);
resume->m_ResumeTime=0;
current->m_Next=resume;
Interrupts::Send(IRQ_TASK_SWITCH, core);
auto parallel=resume->m_Parallel;
while(parallel)
	{
	s_WaitingTask=SuspendTask(s_WaitingTask, parallel, 0);
	auto next=parallel->m_Parallel;
	parallel->m_Parallel=nullptr;
	parallel=next;
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

UINT Scheduler::s_CurrentCore=0;
Handle<Task> Scheduler::s_CurrentTask[CPU_COUNT];
Handle<Task> Scheduler::s_IdleTask[CPU_COUNT];
CriticalSection Scheduler::s_CriticalSection;
Handle<Task> Scheduler::s_WaitingTask;

}
