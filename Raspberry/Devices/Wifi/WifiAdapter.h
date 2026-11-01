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
	inline VOID Command(WifiCommand Id, UINT Argument)
		{
		Command(Id, &Argument, sizeof(UINT));
		}
	VOID Command(WifiCommand Id, VOID* Buffer, SIZE_T Size);
	VOID GetVariable(LPCSTR Name, VOID* Buffer, UINT Size);
	Event<WifiAdapter, Handle<WifiPacket>> PacketReceived;
	Handle<WifiPacket> SendAndReceive(Handle<WifiPacket> Request);
	inline VOID SetVariable(LPCSTR Name, UINT Value)
		{
		SetVariable(Name, &Value, sizeof(UINT));
		}
	VOID SetVariable(LPCSTR Name, VOID const* Buffer, UINT Size);

private:
	// Con-/Destructors
	WifiAdapter();

	// Common
	VOID Initialize();
	UINT InitializeConfiguration(UINT* Buffer, UINT Size);
	VOID OnPacketReceived(Handle<WifiPacket> Packet);
	WIFI_EVENT_MESSAGE* ParseEvent(WifiPacket* Packet);
	VOID ServiceTask();
	VOID UploadRegulatory();
	Mutex m_Mutex;
	Signal m_Ready;
	WifiPacket* m_Request;
	WORD m_RequestId;
	Signal m_ResponseReceived;
	Handle<WifiSdio> m_Sdio;
};

}}