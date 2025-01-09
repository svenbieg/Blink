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
	VOID Lock();
	BOOL TryLock();
	VOID Unlock();

private:
	// Common
	volatile UINT m_Core;
	UINT m_LockCount;
};

}
