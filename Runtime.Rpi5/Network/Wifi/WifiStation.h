//===============
// WifiStation.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Wifi/WifiAdapter.h"
#include "Network/Ethernet/Ethernet.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Wifi {


//==============
// Wifi-Station
//==============

class WifiStation: public Network::Ethernet::Ethernet
{
public:
	// Using
	using Task=Concurrency::Task;
	using WifiAdapter=Devices::Wifi::WifiAdapter;
	using WifiPacket=Devices::Wifi::WifiPacket;

	// Con-/Destructors
	static inline Handle<WifiStation> Create() { return new WifiStation(); }

	// Common
	VOID Connect();
	Handle<String> HostName;
	Handle<String> Network;
	Handle<String> Password;

private:
	// Con-/Destructors
	WifiStation();

	// Common
	VOID ConnectTask();
	VOID OnWifiPacketReceived(WifiPacket* Packet);
	VOID Scan();
	Handle<Task> m_ConnectTask;
	Handle<WifiAdapter> m_Device;
};

}}