//====================
// InterruptHandler.h
//====================

#pragma once


//=======
// Using
//=======

#include "Handle.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//===================
// Interrupt-Handler
//===================

class InterruptHandler
{
public:
	// Con-/Destructors
	virtual ~InterruptHandler()=default;

	// Common
	virtual VOID Run()=0;

protected:
	// Con-/Destructors
	InterruptHandler()=default;
};


//===========
// Procedure
//===========

class InterruptProcedure: public InterruptHandler
{
public:
	// Definitions
	typedef VOID (*_proc_t)();

	// Con-/Destructors
	InterruptProcedure(_proc_t Procedure)noexcept: m_Procedure(Procedure) {}

	// Common
	VOID Run()override { (*m_Procedure)(); }

private:
	// Common
	_proc_t m_Procedure;
};


//==================
// Member-Procedure
//==================

template <class _owner_t>
class InterruptMemberProcedure: public InterruptHandler
{
public:
	// Definitions
	typedef VOID (_owner_t::*_proc_t)();

	// Con-/Destructors
	InterruptMemberProcedure(_owner_t* Owner, _proc_t Procedure)noexcept: m_Owner(Owner), m_Procedure(Procedure) {}

	// Common
	VOID Run()override { (m_Owner->*m_Procedure)(); }

private:
	// Common
	_owner_t* m_Owner;
	_proc_t m_Procedure;
};

}}