//===============
// WifiAdapter.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"
#include "Concurrency/Task.h"
#include "Devices/Wifi/WifiSdio.h"
#include "Global.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//===========
// Interface
//===========

enum class WifiInterface
{
Station=WIFI_STA,
AccessPoint=WIFI_AP
};


//==============
// Wifi-Adapter
//==============

class WifiAdapter: public Global<WifiAdapter>, public WifiSdio
{
public:
	// Friends
	friend Object;

	// Using
	using DispatchedQueue=Concurrency::DispatchedQueue;
	using MAC_ADDR=Network::Ethernet::MAC_ADDR;
	using Mutex=Concurrency::Mutex;
	using Signal=Concurrency::Signal;
	using Task=Concurrency::Task;

	// Con-/Destructors
	static inline Handle<WifiAdapter> Create() { return Global::Create(); }

	// Common
	inline VOID Command(WifiCommand Id, UINT Argument, WifiInterface Interface=WifiInterface::Station)
		{
		Command(Id, &Argument, sizeof(UINT), Interface);
		}
	VOID Command(WifiCommand Id, VOID* Buffer, SIZE_T Size, WifiInterface Interface=WifiInterface::Station);
	VOID Enable();
	VOID EnableWifi();
	VOID GetVariable(LPCSTR Name, VOID* Buffer, UINT Size, WifiInterface Interface=WifiInterface::Station);
	inline VOID SetVariable(LPCSTR Name, UINT Value, WifiInterface Interface=WifiInterface::Station)
		{
		SetVariable(Name, &Value, sizeof(UINT), Interface);
		}
	VOID SetVariable(LPCSTR Name, VOID const* Buffer, UINT Size, WifiInterface Interface=WifiInterface::Station);

private:
	// Flags
	enum class WifiFlags
		{
		None=0,
		SdioEnabled=(1<<0),
		WifiEnabled=(1<<1)
		};

	// Access-Mode
	enum class AccessMode { Locked };

	// Con-/Destructors
	WifiAdapter();

	// Common
	inline VOID Command(AccessMode Access, WifiCommand Id, UINT Argument, WifiInterface Interface=WifiInterface::Station)
		{
		Command(Access, Id, &Argument, sizeof(UINT), Interface);
		}
	VOID Command(AccessMode Access, WifiCommand Id, VOID* Buffer, SIZE_T Size, WifiInterface Interface=WifiInterface::Station);
	VOID Disconnect();
	VOID GetVariable(AccessMode Access, LPCSTR Name, VOID* Buffer, UINT Size, WifiInterface Interface=WifiInterface::Station);
	UINT InitializeConfiguration(BYTE* Buffer, UINT Size);
	VOID OnResponseReceived(WifiPacket* Packet);
	VOID OnPacketReceived(Handle<WifiPacket> Packet)override;
	inline VOID SetVariable(AccessMode Access, LPCSTR Name, UINT Value, WifiInterface Interface=WifiInterface::Station)
		{
		SetVariable(Access, Name, &Value, sizeof(UINT), Interface);
		}
	VOID SetVariable(AccessMode Access, LPCSTR Name, VOID const* Buffer, UINT Size, WifiInterface Interface=WifiInterface::Station);
	WIFI_EVENT_MESSAGE* ParseEvent(WifiPacket* Packet);
	Handle<WifiPacket> Transfer(WifiPacket* Request);
	VOID UploadRegulatory();
	WifiFlags m_Flags;
	Mutex m_Mutex;
	Signal m_Ready;
	WifiPacket* m_Request;
	WORD m_RequestId;
	Signal m_ResponseReceived;
};

}}