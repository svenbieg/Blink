//============
// EmmcHost.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
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
	using Irq=Devices::System::Irq;

	// Con-/Destructors
	~EmmcHost();
	static inline Handle<EmmcHost> Create(SIZE_T Address, Irq Irq) { return new EmmcHost(Address, Irq); }

	// Common
	UINT AppCommand(UINT Command, UINT Argument=0, UINT Timeout=100);
	VOID AppCommand(UINT Command, UINT Argument, UINT* Response, UINT Timeout=100);
	VOID AppCommand(UINT Command, UINT Argument, UINT* Response, UINT* Buffer, UINT BlockCount, UINT BlockSize, UINT Timeout=100);
	UINT Command(UINT Command, UINT Argument=0, UINT Timeout=100);
	VOID Command(UINT Command, UINT Argument, UINT* Response, UINT Timeout=100);
	VOID Command(UINT Command, UINT Argument, UINT* Response, UINT* Buffer, UINT BlockCount, UINT BlockSize, UINT Timeout=100);
	VOID PollRegister(UINT Function, UINT Address, BYTE Mask, BYTE Value, UINT Timeout=100);
	VOID PowerOff();
	BYTE ReadRegister(UINT Function, UINT Address);
	VOID Reset();
	VOID SelectCard(UINT RelativeCardAddress);
	VOID SetClockRate(UINT BaseClock, UINT ClockRate);
	VOID SetRegister(UINT Function, UINT Address, UINT Mask);
	VOID WriteRegister(UINT Function, UINT Address, BYTE Value);

private:
	// Con-/Destructors
	EmmcHost(SIZE_T Address, Irq Irq);

	// Common
	static VOID HandleInterrupt(VOID* Parameter);
	VOID OnInterrupt();
	SIZE_T m_Address;
	Concurrency::CriticalSection m_CriticalSection;
	Irq m_Irq;
	volatile UINT m_IrqFlags;
	UINT m_RelativeCardAddress;
	Concurrency::Signal m_Signal;
};

}}