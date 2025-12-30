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
	using CriticalSection=Concurrency::CriticalSection;
	using Irq=Devices::System::Irq;
	using Signal=Concurrency::Signal;

	// Con-/Destructors
	~EmmcHost();
	static inline Handle<EmmcHost> Create(SIZE_T Address, Irq Irq) { return new EmmcHost(Address, Irq); }

	// Common
	UINT Command(EmmcCmd Command, UINT Argument=0);
	VOID Command(EmmcCmd Command, UINT Argument, UINT* Response);
	VOID Command(EmmcCmd Command, UINT Argument, UINT* Response, VOID* Buffer, UINT BlockCount, UINT BlockSize);
	UINT Command(EmmcAppCmd Command, UINT Argument=0);
	VOID Command(EmmcAppCmd Command, UINT Argument, UINT* Response);
	VOID Command(EmmcAppCmd Command, UINT Argument, UINT* Response, VOID* Buffer, UINT BlockCount, UINT BlockSize);
	inline EMMC_REGS* GetDevice()const { return m_Device; }
	VOID IoRwExtended(EMMC_FN const& Function, IoRwFlags Flags, UINT Address, VOID* Buffer, UINT Size);
	VOID PollRegister(EMMC_REG const& Register, BYTE Mask, BYTE Value, UINT Timeout=100);
	VOID PowerOff();
	BYTE ReadRegister(EMMC_REG const& Register);
	VOID SelectCard(UINT RelativeCardAddress);
	VOID SetClockRate(UINT BaseClock, UINT ClockRate);
	VOID SetRegister(EMMC_REG const& Register, BYTE Mask);
	VOID SetRegister(EMMC_REG const& Register, BYTE Mask, BYTE Value);
	VOID WriteRegister(EMMC_REG const& Register, BYTE Value);

private:
	// Con-/Destructors
	EmmcHost(SIZE_T Address, Irq Irq);

	// Common
	static VOID HandleInterrupt(VOID* Parameter);
	VOID OnInterrupt();
	EMMC_REGS* m_Device;
	CriticalSection m_CriticalSection;
	Irq m_Irq;
	volatile UINT m_IrqFlags;
	UINT m_RelativeCardAddress;
	Signal m_Signal;
};

}}