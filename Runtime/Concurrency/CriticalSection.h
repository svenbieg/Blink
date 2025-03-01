//===================
// CriticalSection.h
//===================

#pragma once


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class SpinLock;


//==================
// Critical-Section
//==================

class CriticalSection
{
public:
	// Friends
	friend SpinLock;

	// Con-/Destructors
	CriticalSection();

	// Common
	VOID Lock();
	BOOL TryLock();
	VOID Unlock();
	VOID Yield();

private:
	// Common
	volatile UINT m_Core;
	UINT m_LockCount;
};

}