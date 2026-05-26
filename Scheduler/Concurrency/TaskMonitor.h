//===============
// TaskMonitor.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/System/Cpu.h"


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
SIZE_T AllocSize;
Handle<String> Name;
SIZE_T StackSize;
SIZE_T StackUsed;
UINT64 TotalTime;
}TASK_INFO;


//==============
// Task-Monitor
//==============

class TaskMonitor
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

	// Common
	static UINT GetTaskInfo(TASK_INFO* TaskInfo, UINT Count);

private:
	// Common
	static VOID Allocate(VOID* Buffer);
	static VOID ClearInterrupt(UINT Core);
	static VOID Free(VOID* Buffer)noexcept;
	static VOID Initialize();
	static VOID RemoveTask(Task* Task);
	static VOID SetInterrupt(UINT Core);
	static VOID SetTask(UINT Core, Task* Next);
	static Task* s_Current[Cpu::CPU_COUNT];
	static UINT64 s_IrqStart[Cpu::CPU_COUNT];
	static UINT64 s_TotalTime;
};

}