//============
// EmmcHost.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/Emmc/Emmc.h"
#include "Devices/System/Interrupts.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Emmc {


//======
// Host
//======

class EmmcHost: public Object
{
public:
	// Using
	using CriticalSection=Concurrency::CriticalSection;
	using Irq=Devices::System::Irq;
	using Signal=Concurrency::Signal;
	using SpinLock=Concurrency::SpinLock;

	// Con-/Destructors
	~EmmcHost();
	static inline Handle<EmmcHost> Create(SIZE_T Address, Irq Irq) { return new EmmcHost(Address, Irq); }

	// Common
	UINT Command(EmmcCmd Command, UINT Argument=0);
	VOID Command(EmmcCmd Command, UINT Argument, UINT* Response, VOID* Buffer=nullptr, UINT BlockCount=0, UINT BlockSize=0);
	UINT Command(EmmcAppCmd Command, UINT Argument=0);
	VOID Command(EmmcAppCmd Command, UINT Argument, UINT* Response, VOID* Buffer=nullptr, UINT BlockCount=0, UINT BlockSize=0);
	inline EMMC_REGS* GetDevice()const { return m_Device; }
	VOID PowerOff();
	VOID SelectCard(UINT RelativeCardAddress);
	VOID SetClockRate(UINT BaseClock, UINT ClockRate);

protected:
	// Con-/Destructors
	EmmcHost(SIZE_T Address, Irq Irq);

	// Common
	VOID Command(SpinLock& Lock, EmmcCmd Command, UINT Argument, UINT* Response=nullptr, VOID* Buffer=nullptr, UINT BlockCount=0, UINT BlockSize=0);
	VOID HandleInterrupt();
	UINT m_BlockSize;
	UINT* m_Buffer;
	Signal m_CardIrq;
	Signal m_CommandDone;
	CriticalSection m_CriticalSection;
	Signal m_DataDone;
	EMMC_REGS* m_Device;
	Irq m_Irq;
	UINT m_OutputCount;
	UINT m_RelativeCardAddress;
};

}}