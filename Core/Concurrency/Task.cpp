//==========
// Task.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "Core/Application.h"
#include "Scheduler.h"
#include "Task.h"

using namespace Core;


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

Status Task::Wait()
{
ScopedLock lock(m_Mutex);
m_Done.Wait(lock);
return m_Status;
}


//============================
// Con-/Destructors Protected
//============================

Task::Task():
m_Status(Status::Pending),
// Private
m_Flags(TaskFlags::None),
m_ResumeTime(0),
m_StackPointer(&m_Stack[STACK_SIZE])
{}


//================
// Common Private
//================

VOID Task::TaskProc(VOID* param)
{
auto task=(Task*)param;
Status status=Status::Success;
try
	{
	task->Run();
	}
catch(Exception e)
	{
	status=e.GetStatus();
	}
task->m_Status=status;
task->m_Done.Broadcast();
if(task->m_Then)
	{
	if(Application::Current)
		Application::Current->Dispatch(task->m_Then);
	}
Scheduler::ExitTask();
}

}
