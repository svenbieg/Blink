//============
// EmmcHost.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/ServiceMutex.h"
#include "Concurrency/Signal.h"
#include "Concurrency/Task.h"
#include "Devices/Emmc/Emmc.h"
#include "Devices/System/Interrupts.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Emmc {


//========
// Device
//========

enum class EmmcDevice
{
SdCard,
Wifi
};


//======
// Host
//======

class EmmcHost: public virtual Object
{
public:
	// Using
	using CriticalSection=Concurrency::CriticalSection;
	using Irq=Devices::System::Irq;
	using ServiceMutex=Concurrency::ServiceMutex;
	using Signal=Concurrency::Signal;
	using SpinLock=Concurrency::SpinLock;
	using Task=Concurrency::Task;
	using WriteLock=Concurrency::WriteLock;

	// Friends
	friend Object;

	// Con-/Destructors
	~EmmcHost();
	static inline Handle<EmmcHost> Create(EmmcDevice Device)
		{
		return Object::Create<EmmcHost>(Device);
		}

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
	EmmcHost(EmmcDevice Device);

	// Common
	VOID Command(WriteLock& Lock, EmmcCmd Command, UINT Argument, UINT* Response=nullptr, VOID* Buffer=nullptr, UINT BlockCount=0, UINT BlockSize=0);
	VOID HandleInterrupt();
	VOID ServiceTask();
	UINT m_BlockSize;
	UINT* m_Buffer;
	Signal m_CardIrq;
	Signal m_CommandDone;
	CriticalSection m_CriticalSection;
	Signal m_DataDone;
	EMMC_REGS* m_Device;
	Signal m_InterruptPending;
	Irq m_Irq;
	ServiceMutex m_Mutex;
	UINT m_OutputCount;
	UINT m_RelativeCardAddress;
	Handle<Task> m_ServiceTask;
};

}}