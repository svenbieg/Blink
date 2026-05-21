//=================
// TaskMonitor.cpp
//=================

#include "TaskMonitor.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/Timers/SystemTimer.h"
#include "Runtime/Configuration.h"

using namespace Devices::Timers;


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Con-/Destructors
//==================

TaskMonitor::~TaskMonitor()
{
Interrupts::SetTaskMonitor(nullptr);
Scheduler::SetTaskMonitor(nullptr);
}


//========
// Common
//========

UINT TaskMonitor::GetTaskInfo(TASK_INFO* info, UINT count)
{
if(!info||!count)
	throw InvalidArgumentException();
SpinLock lock(Scheduler::s_CriticalSection);
UINT64 time=SystemTimer::Microseconds64();
for(UINT core=0; core<Scheduler::CPU_COUNT; core++)
	{
	if(!m_Current[core])
		continue;
	SetTask(core, m_Current[core]);
	}
UINT64 total_time=0;
UINT pos=0;
Task* task=Scheduler::s_All.First();
while(task)
	{
	UINT64 task_time=task->m_TotalTime;
	info[pos].Name=task->m_Name;
	info[pos].StackSize=task->m_StackSize;
	info[pos].StackUsed=task->m_StackUsed;
	info[pos].TotalTime=task_time;
	total_time+=task_time;
	if(++pos==count)
		throw BufferOverrunException();
	task->m_TotalTime=0;
	task=Scheduler::s_All.Next(task);
	}
lock.Unlock();
if(pos>=count)
	throw BufferOverrunException();
info[pos].Name="system";
info[pos].StackSize=Runtime::CONFIG_STACK_SIZE;
info[pos].StackUsed=0;
info[pos].TotalTime=m_TotalTime-total_time;
m_TotalTime=0;
return pos+1;
}


//==========================
// Con-/Destructors Private
//==========================

TaskMonitor::TaskMonitor():
m_Current({ nullptr }),
m_IrqStart({ 0 }),
m_TotalTime(0)
{
SpinLock lock(Scheduler::s_CriticalSection);
Task* task=Scheduler::s_All.First();
while(task)
	{
	task->m_TotalTime=0;
	task=Scheduler::s_All.Next(task);
	}
UINT64 time=SystemTimer::Microseconds64();
for(UINT u=0; u<Scheduler::CPU_COUNT; u++)
	{
	auto current=Scheduler::s_CurrentTask[u];
	if(!current)
		continue;
	current->m_StartTime=time;
	m_Current[u]=current;
	}
Scheduler::s_TaskMonitor=this;
Interrupts::SetTaskMonitor(this);
}


//================
// Common Private
//================

VOID TaskMonitor::ClearInterrupt(UINT core)
{
UINT64 time=SystemTimer::Microseconds64();
UINT64 irq_time=time-m_IrqStart[core];
m_Current[core]->m_StartTime+=irq_time;
m_TotalTime+=irq_time;
}

VOID TaskMonitor::RemoveTask(Task* task)
{
m_TotalTime-=task->m_TotalTime;
}

VOID TaskMonitor::SetInterrupt(UINT core)
{
m_IrqStart[core]=SystemTimer::Microseconds64();
}

VOID TaskMonitor::SetTask(UINT core, Task* next)
{
UINT64 time=SystemTimer::Microseconds64();
UINT64 task_time=time-m_Current[core]->m_StartTime;
SIZE_T stack_ptr=m_Current[core]->m_StackPointer;
SIZE_T stack_top=m_Current[core]->m_StackTop;
SIZE_T stack_used=m_Current[core]->m_StackUsed;
m_Current[core]->m_StackUsed=TypeHelper::Max(stack_used, stack_top-stack_ptr);
m_Current[core]->m_TotalTime+=task_time;
m_TotalTime+=task_time;
next->m_StartTime=time;
m_Current[core]=next;
}

}