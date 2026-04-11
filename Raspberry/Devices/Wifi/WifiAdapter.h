//===============
// WifiAdapter.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/CriticalMutex.h"
#include "Concurrency/DispatchedQueue.h"
#include "Concurrency/Task.h"
#include "Devices/Wifi/WifiSdio.h"
#include "Event.h"
#include "Global.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==============
// Wifi-Adapter
//==============

class WifiAdapter: public Global<WifiAdapter>
{
public:
	// Friends
	friend Object;
	friend WifiSdio;

	// Using
	using DispatchedQueue=Concurrency::DispatchedQueue;
	using Mutex=Concurrency::Mutex;
	using Signal=Concurrency::Signal;

	// Con-/Destructors
	static inline Handle<WifiAdapter> Create() { return Global::Create(); }

	// Common
	VOID Command(WifiCommand Command, UINT Argument);
	VOID GetVariable(LPCSTR Name, VOID* Buffer, UINT Size);
	Event<WifiAdapter, WifiPacket*> PacketReceived;
	Signal Ready;
	Handle<WifiPacket> SendAndReceive(Handle<WifiPacket> Request);
	inline VOID SetVariable(LPCSTR Name, UINT Value) { SetVariable(Name, &Value, sizeof(UINT)); }
	VOID SetVariable(LPCSTR Name, VOID const* Buffer, UINT Size);

private:
	// Using
	using PacketList=LINKED_LIST(WifiPacket, m_Link);

	// Con-/Destructors
	WifiAdapter();

	// Common
	VOID Initialize();
	UINT InitializeConfiguration(UINT* Buffer, UINT Size);
	VOID OnPacketReceived(WifiPacket* Packet);
	VOID ServiceTask();
	VOID UploadRegulatory();
	MAC_ADDR m_MacAddress;
	Mutex m_Mutex;
	WifiPacket* m_Request;
	WORD m_RequestId;
	PacketList m_Requests;
	Signal m_ResponseReceived;
	Handle<WifiSdio> m_Sdio;
};

}}