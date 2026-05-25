//===============
// TaskMonitor.h
//===============

#pragma once


//=======
// Using
//=======

#include "Collections/map.hpp"
#include "Storage/Streams/OutputStream.h"
#include "Concurrency/Task.h"
#include "Devices/System/Cpu.h"
#include "Global.h"


//======================
// Forward-Declarations
//======================

namespace Concurrency
{
class Scheduler;
class TaskMonitor;
}

namespace Devices
{
namespace System
	{
	class Interrupts;
	class Memory;
	}
}


//===========
// Namespace
//===========

namespace Concurrency {


//===========
// Task-Info
//===========

typedef struct
{
UINT AllocCount;
SIZE_T AllocSize;
Handle<String> Name;
SIZE_T StackSize;
SIZE_T StackUsed;
UINT64 TotalTime;
}TASK_INFO;


//==============
// Task-Monitor
//==============

class TaskMonitor: public Global<TaskMonitor>
{
public:
	// Using
	using Cpu=Devices::System::Cpu;
	using Interrupts=Devices::System::Interrupts;
	using Memory=Devices::System::Memory;
	using OutputStream=Storage::Streams::OutputStream;

	// Friends
	friend Interrupts;
	friend Memory;
	friend Object;
	friend Scheduler;

	// Con-/Destructors
	static inline Handle<TaskMonitor> Create() { return Global::Create(); }
	~TaskMonitor();

	// Common
	UINT GetTaskInfo(TASK_INFO* TaskInfo, UINT Count);

private:
	// Con-/Destructors
	TaskMonitor();

	// Common
	VOID Allocate(SIZE_T Size);
	VOID ClearInterrupt(UINT Core);
	VOID Free(SIZE_T Size);
	VOID RemoveTask(Task* Task);
	VOID SetInterrupt(UINT Core);
	VOID SetTask(UINT Core, Task* Next);
	Task* m_Current[Cpu::CPU_COUNT];
	UINT64 m_IrqStart[Cpu::CPU_COUNT];
	UINT64 m_TotalTime;
};

}