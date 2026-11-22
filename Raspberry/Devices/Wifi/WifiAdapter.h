//===============
// WifiAdapter.h
//===============

#pragma once


//=======
// Using
//=======

#include "Devices/Wifi/WifiFirmware.h"
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

class WifiAdapter: public Global<WifiAdapter>, private WifiSdio
{
public:
	// Friends
	friend Object;
	friend WifiFirmware;

	// Using
	using Mutex=Concurrency::Mutex;
	using Signal=Concurrency::Signal;
	using WriteLock=Concurrency::WriteLock;

	// Con-/Destructors
	static inline Handle<WifiAdapter> Create() { return Global::Create(); }

	// Common
	VOID Command(WifiCommand Id, UINT Argument, WifiInterface Interface=WifiInterface::Station);
	VOID Command(WifiCommand Id, VOID* Buffer, SIZE_T Size, WifiInterface Interface=WifiInterface::Station);
	VOID EnableWifi();
	VOID GetVariable(LPCSTR Name, VOID* Buffer, UINT Size, WifiInterface Interface=WifiInterface::Station);
	VOID SetVariable(LPCSTR Name, UINT Value, WifiInterface Interface=WifiInterface::Station);
	VOID SetVariable(LPCSTR Name, VOID const* Buffer, UINT Size, WifiInterface Interface=WifiInterface::Station);

private:
	// Flags
	enum class WifiFlags
		{
		None=0,
		Enabled=(1<<0)
		};

	// Con-/Destructors
	WifiAdapter();

	// Common
	VOID Command(WriteLock const& Lock, WifiCommand Id, UINT Argument, WifiInterface Interface=WifiInterface::Station);
	VOID Command(WriteLock const& Lock, WifiCommand Id, VOID* Buffer, SIZE_T Size, WifiInterface Interface=WifiInterface::Station);
	VOID Disconnect();
	VOID GetVariable(WriteLock const& Lock, LPCSTR Name, VOID* Buffer, UINT Size, WifiInterface Interface=WifiInterface::Station);
	VOID OnDataReceived(WifiPacket* Packet);
	VOID OnEventReceived(WIFI_EVENT_MESSAGE* Message);
	VOID OnPacketReceived(Handle<WifiPacket> Packet)override;
	VOID OnResponseReceived(WifiPacket* Packet);
	VOID SetVariable(WriteLock const& Lock, LPCSTR Name, UINT Value, WifiInterface Interface=WifiInterface::Station);
	VOID SetVariable(WriteLock const& Lock, LPCSTR Name, VOID const* Buffer, UINT Size, WifiInterface Interface=WifiInterface::Station);
	WIFI_EVENT_MESSAGE* ParseEvent(WifiPacket* Packet);
	Handle<WifiPacket> Transfer(WifiPacket* Request);
	WifiFlags m_Flags;
	Mutex m_Mutex;
	WifiPacket* m_Request;
	WORD m_RequestId;
	Signal m_ResponseReceived;
};

}}