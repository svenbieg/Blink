//===============
// WifiAdapter.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Emmc/EmmcHost.h"
#include "Devices/Wifi/WifiPacket.h"
#include "Network/Ethernet/MacAddress.h"
#include "Event.h"
#include <span>


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==============
// Wifi-Adapter
//==============

class WifiAdapter: public Object
{
public:
	// Using
	using EMMC_FN=Devices::Emmc::EMMC_FN;
	using EMMC_REGS=Devices::Emmc::EMMC_REGS;
	using EmmcHost=Devices::Emmc::EmmcHost;
	using MAC_ADDR=Network::Ethernet::MAC_ADDR;
	using Mutex=Concurrency::Mutex;
	using PacketBuffer=Storage::PacketBuffer;
	using ScopedLock=Concurrency::ScopedLock;
	using Signal=Concurrency::Signal;
	using Task=Concurrency::Task;

	// Con-/Destructors
	~WifiAdapter();
	static inline Handle<WifiAdapter> Create() { return Object::Create<WifiAdapter>(); }
	VOID Initialize();
	Event<WifiAdapter, WifiPacket*> PacketReceived;
	VOID Send(WifiPacket* Packet);
	Handle<WifiPacket> SendAndReceive(WifiPacket* Packet);

private:
	// Con-/Destructors
	friend Object;
	WifiAdapter();

	// Common
	INT GetInt(WifiCmd Id);
	INT GetInt(LPCSTR Name);
	UINT GetVariable(LPCSTR Name, VOID* Buffer, UINT Size);
	VOID HandleFrame();
	UINT InitializeConfiguration(BYTE* Buffer, UINT Size);
	VOID IoPoll(EMMC_FN Function, UINT Address, UINT Mask, UINT Value, UINT Timeout=100);
	UINT IoRead(EMMC_FN Function, UINT Address);
	UINT IoRead(EMMC_FN Function, UINT Address, VOID* Buffer, UINT Size);
	VOID IoWindow(UINT Address);
	VOID IoWrite(EMMC_FN Function, UINT Address, UINT Value);
	VOID IoWrite(EMMC_FN Function, UINT Address, VOID const* Buffer, UINT Size);
	VOID Reset(UINT Address, UINT Flags);
	VOID ServiceTask();
	VOID SetInt(WifiCmd Id, INT Value);
	VOID SetInt(LPCSTR Name, INT Value);
	VOID SetVariable(LPCSTR Name, VOID const* Buffer, UINT Size);
	VOID UploadRegulatory();
	VOID WaitForInterrupt(ScopedLock& Lock);
	VOID WifiRead(VOID* Buffer, UINT Size);
	VOID WifiWrite(VOID const* Buffer, UINT Size);
	EMMC_REGS* m_Device;
	Handle<EmmcHost> m_EmmcHost;
	UINT m_IoWindow;
	MAC_ADDR m_MacAddress;
	Mutex m_Mutex;
	WORD m_RequestId;
	Handle<WifiPacket> m_Response;
	Signal m_ResponseReceived;
	Handle<Task> m_ServiceTask;
};

}}