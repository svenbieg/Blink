//============
// WifiSdio.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/ServiceMutex.h"
#include "Devices/Wifi/WifiPacket.h"
#include "Devices/Wifi/WifiSpi.h"
#include "Devices/Pio/SpiEmulator.h"


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

class WifiSdio: public Pio::SpiEmulator
{
public:
	// Friends
	friend WifiAdapter;

	// Using
	using CriticalSection=Concurrency::CriticalSection;
	using ServiceMutex=Concurrency::ServiceMutex;
	using Signal=Concurrency::Signal;
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
	VOID HandleInterrupt();
	virtual VOID OnPacketReceived(Handle<WifiPacket> Packet)=0;
	VOID PollBackplane(UINT Address, UINT Mask, UINT Value, UINT Timeout=100);
	VOID PollRegister(SDIO_REG const& Register, UINT Mask, UINT Value, UINT Timeout=100);
	UINT Read16x2(FN0_32 const& Register);
	UINT ReadBackplane(UINT Address);
	VOID ReadBackplane(UINT Address, VOID* Buffer, UINT Size);
	SIZE_T ReadData(VOID* Buffer, SIZE_T Size);
	VOID ReadPackets();
	UINT ReadRegister(SDIO_REG const& Register);
	VOID ResetDevice(UINT Address, UINT Flags);
	VOID ServiceTask();
	VOID SetBackplane(UINT Address);
	VOID SetRegister(SDIO_REG const& Register, UINT Mask, UINT Value);
	VOID Write16x2(FN0_32 const& Register, UINT Value);
	VOID WriteBackplane(UINT Address, UINT Value);
	VOID WriteBackplane(UINT Address, VOID const* Buffer, UINT Size);
	SIZE_T WriteData(VOID const* Buffer, SIZE_T Size);
	VOID WriteRegister(SDIO_REG const& Register, UINT Value);
	CriticalSection m_CriticalSection;
	Signal m_InterruptPending;
	Handle<Task> m_ServiceTask;
	UINT m_Window;
};

}}