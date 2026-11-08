//============
// WifiSdio.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/ServiceMutex.h"
#include "Devices/Emmc/EmmcHost.h"
#include "Devices/Wifi/WifiPacket.h"
#include "Storage/Streams/RandomAccessStream.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//===========
// Wifi-Sdio
//===========

class WifiSdio: public Emmc::EmmcHost
{
public:
	// Using
	using ServiceMutex=Concurrency::ServiceMutex;
	using Task=Concurrency::Task;

	// Con-/Destructors
	~WifiSdio();

protected:
	// Con-/Destructors
	WifiSdio();

	// Common
	VOID EnableSdio();
	VOID WritePacket(WifiPacket* Packet);
	ServiceMutex m_SdioMutex;

private:
	// Common
	UINT IoRwExtended(UINT Function, UINT Flags, UINT Address, BYTE* Buffer, UINT Size);
	virtual VOID OnPacketReceived(Handle<WifiPacket> Packet)=0;
	VOID PollBackplane(UINT Address, UINT Mask, UINT Value, UINT Timeout=100);
	VOID PollRegister(SDIO_REG const& Register, BYTE Mask, BYTE Value, UINT Timeout=100);
	UINT ReadBackplane(UINT Address);
	VOID ReadBackplane(UINT Address, UINT* Buffer, UINT Size);
	SIZE_T ReadData(VOID* Buffer, SIZE_T Size);
	BYTE ReadRegister(SDIO_REG const& Register);
	VOID ResetDevice(UINT Address, UINT Flags);
	VOID ServiceTask();
	VOID SetBackplane(UINT Address);
	inline VOID SetRegister(SDIO_REG const& Register, BYTE Mask) { SetRegister(Register, Mask, Mask); }
	VOID SetRegister(SDIO_REG const& Register, BYTE Mask, BYTE Value);
	VOID WriteBackplane(UINT Address, UINT Value);
	VOID WriteBackplane(UINT Address, UINT const* Buffer, UINT Size);
	SIZE_T WriteData(VOID const* Buffer, SIZE_T Size);
	VOID WriteRegister(SDIO_REG const& Register, BYTE Value);
	Handle<Task> m_ServiceTask;
	UINT m_Window;
};

}}