//=======================
// DispatchedHandler.cpp
//=======================

#include "DispatchedHandler.h"


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

Status DispatchedHandler::GetStatus()
{
SpinLock lock(m_CriticalSection);
return m_Status;
}

VOID DispatchedHandler::Wait()
{
SpinLock lock(m_CriticalSection);
if(m_Status==Status::Pending)
	m_Signal.Wait(lock);
}


//============================
// Con-/Destructors Protected
//============================

DispatchedHandler::DispatchedHandler():
m_Status(Status::Pending)
{}


//===========
// Procedure
//===========

VOID DispatchedProcedure::Run()
{
Status status=Status::Success;
try
	{
	(*m_Procedure)();
	}
catch(Exception e)
	{
	status=e.GetStatus();
	}
SpinLock lock(m_CriticalSection);
m_Status=status;
m_Signal.Trigger();
}

}