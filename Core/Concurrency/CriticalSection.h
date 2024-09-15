//===================
// CriticalSection.h
//===================

#pragma once


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Critical-Section
//==================

class CriticalSection
{
public:
	// Con-/Destructors
	CriticalSection();

	// Common
	VOID Enter();
	BOOL TryEnter();
	VOID Leave();

private:
	// Common
	volatile UINT m_Core;
	UINT m_LockCount;
};

}
