//============
// WifiSdio.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Emmc/EmmcHost.h"
#include "Devices/Wifi/Wifi.h"
#include "Event.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//===========
// Wifi-Sdio
//===========

class WifiSdio: public Emmc::EmmcHost, public Storage::Streams::RandomAccessStream
{
public:
	// Friends
	friend Object;

	// Using
	using Task=Concurrency::Task;

	// Con-/Destructors
	static inline Handle<WifiSdio> Create() { return Object::Create<WifiSdio>(); }
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

	// Common
	UINT IoRwExtended(UINT Function, UINT Flags, UINT Address, BYTE* Buffer, UINT Size);
	VOID PollBackplane(UINT Address, UINT Mask, UINT Value, UINT Timeout=100);
	VOID PollRegister(SDIO_REG const& Register, BYTE Mask, BYTE Value, UINT Timeout=100);
	UINT ReadBackplane(UINT Address);
	VOID ReadBackplane(UINT Address, UINT* Buffer, UINT Size);
	BYTE ReadRegister(SDIO_REG const& Register);
	VOID ResetDevice(UINT Address, UINT Flags);
	VOID ServiceTask();
	VOID SetBackplane(UINT Address);
	inline VOID SetRegister(SDIO_REG const& Register, BYTE Mask) { SetRegister(Register, Mask, Mask); }
	VOID SetRegister(SDIO_REG const& Register, BYTE Mask, BYTE Value);
	VOID WriteBackplane(UINT Address, UINT Value);
	VOID WriteBackplane(UINT Address, UINT const* Buffer, UINT Size);
	VOID WriteRegister(SDIO_REG const& Register, BYTE Value);
	Handle<Task> m_ServiceTask;
	UINT m_Window;
};

}}