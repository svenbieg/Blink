//============
// WifiSdio.h
//============

#pragma once


//=======
// Using
//=======

#include "Devices/Wifi/WifiPacket.h"
#include "Devices/Wifi/WifiSpi.h"
#include "Storage/Streams/RandomAccessStream.h"
#include "Event.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//======================
// Forward-Declarations
//======================

class WifiAdapter;


//===========
// Wifi-Sdio
//===========

class WifiSdio: public WifiSpi, public Storage::Streams::RandomAccessStream
{
public:
	// Friends
	friend Object;
	friend WifiAdapter;

	// Using
	using CriticalSection=Concurrency::CriticalSection;
	using ScopedLock=Concurrency::ScopedLock;
	using Signal=Concurrency::Signal;
	using Task=Concurrency::Task;

	// Con-/Destructors
	~WifiSdio();

	// Common
	Event<WifiSdio, WifiPacket*> PacketReceived;

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Con-/Destructors
	WifiSdio();
	static inline Handle<WifiSdio> Create() { return Object::Create<WifiSdio>(); }

	// Common
	VOID HandleInterrupt();
	VOID PollBackplane(UINT Address, UINT Mask, UINT Value, UINT Timeout=100);
	VOID PollRegister(SDIO_REG const& Register, UINT Mask, UINT Value, UINT Timeout=100);
	UINT Read16x2(FN0_32 const& Register);
	UINT ReadBackplane(UINT Address);
	VOID ReadBackplane(UINT Address, UINT* Buffer, UINT Size);
	UINT ReadRegister(SDIO_REG const& Register);
	VOID ResetDevice(UINT Address, UINT Flags);
	VOID ServiceTask();
	VOID SetBackplane(UINT Address);
	VOID SetRegister(SDIO_REG const& Register, UINT Mask, UINT Value);
	VOID Write16x2(FN0_32 const& Register, UINT Value);
	VOID WriteBackplane(UINT Address, UINT Value);
	VOID WriteBackplane(UINT Address, UINT const* Buffer, UINT Size);
	VOID WriteRegister(SDIO_REG const& Register, UINT Value);
	CriticalSection m_CriticalSection;
	Signal m_InterruptPending;
	Handle<Task> m_ServiceTask;
	UINT m_Window;
};

}}