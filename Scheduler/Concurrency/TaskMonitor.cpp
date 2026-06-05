//=================
// TaskMonitor.cpp
//=================

#include "TaskMonitor.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/System/Memory.h"
#include "Devices/Timers/SystemTimer.h"
#include "Runtime/Configuration.h"
#include <heap.h>

using namespace Devices::Timers;


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

UINT TaskMonitor::GetTaskInfo(TASK_INFO* info, UINT count)
{
if(!info||!count)
	throw InvalidArgumentException();
SpinLock lock(Scheduler::s_CriticalSection);
UINT64 time=SystemTimer::Microseconds();
for(UINT core=0; core<Scheduler::CPU_COUNT; core++)
	{
	if(!s_Current[core])
		continue;
	SetTask(core, s_Current[core]);
	}
UINT64 total_time=0;
UINT pos=0;
Task* task=Scheduler::s_All.First();
while(task)
	{
	UINT64 task_time=task->m_TotalTime;
	info[pos].AllocSize=task->m_AllocSize;
	info[pos].Name=task->m_Name;
	info[pos].StackSize=task->m_StackSize;
	info[pos].StackUsed=task->m_StackUsed;
	info[pos].TotalTime=task_time;
	total_time+=task_time;
	if(++pos==count)
		throw BufferOverrunException();
	task->m_AllocSize=0;
	task->m_TotalTime=0;
	task=Scheduler::s_All.Next(task);
	}
lock.Unlock();
if(pos>=count)
	throw BufferOverrunException();
info[pos].AllocSize=0;
info[pos].Name="system";
info[pos].StackSize=Runtime::CONFIG_STACK_SIZE;
info[pos].StackUsed=0;
info[pos].TotalTime=s_TotalTime-total_time;
s_TotalTime=0;
return pos+1;
}


//================
// Common Private
//================

VOID TaskMonitor::Allocate(VOID* buf)
{
UINT core=Cpu::GetId();
heap_block_info_t info;
heap_block_get_info((heap_t*)Memory::s_Heap, buf, &info);
s_Current[core]->m_AllocSize+=info.size;
}

VOID TaskMonitor::ClearInterrupt(UINT core)
{
UINT64 time=SystemTimer::Microseconds();
UINT64 irq_time=time-s_IrqStart[core];
s_Current[core]->m_StartTime+=irq_time;
s_TotalTime+=irq_time;
}

VOID TaskMonitor::Free(VOID* buf)noexcept
{
UINT core=Cpu::GetId();
heap_block_info_t info;
heap_block_get_info((heap_t*)Memory::s_Heap, buf, &info);
s_Current[core]->m_AllocSize-=info.size;
}

VOID TaskMonitor::Initialize()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT64 time=SystemTimer::Microseconds();
for(UINT u=0; u<Scheduler::CPU_COUNT; u++)
	{
	auto current=Scheduler::s_CurrentTask[u];
	if(!current)
		continue;
	current->m_StartTime=time;
	s_Current[u]=current;
	}
}

VOID TaskMonitor::RemoveTask(Task* task)
{
s_TotalTime-=task->m_TotalTime;
}

VOID TaskMonitor::SetInterrupt(UINT core)
{
s_IrqStart[core]=SystemTimer::Microseconds();
}

VOID TaskMonitor::SetTask(UINT core, Task* next)
{
UINT64 time=SystemTimer::Microseconds();
if(s_Current[core])
	{
	UINT64 task_time=time-s_Current[core]->m_StartTime;
	SIZE_T stack_ptr=s_Current[core]->m_StackPointer;
	SIZE_T stack_top=s_Current[core]->m_StackTop;
	SIZE_T stack_used=s_Current[core]->m_StackUsed;
	s_Current[core]->m_StackUsed=TypeHelper::Max(stack_used, stack_top-stack_ptr);
	s_Current[core]->m_TotalTime+=task_time;
	s_TotalTime+=task_time;
	}
next->m_StartTime=time;
s_Current[core]=next;
}

Task* TaskMonitor::s_Current[Cpu::CPU_COUNT]={ nullptr };
UINT64 TaskMonitor::s_IrqStart[Cpu::CPU_COUNT]={ 0 };
UINT64 TaskMonitor::s_TotalTime=0;

}