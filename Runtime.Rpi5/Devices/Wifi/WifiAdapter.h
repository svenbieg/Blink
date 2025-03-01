//===============
// WifiAdapter.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Emmc/EmmcHost.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//=========
// Network
//=========

typedef struct
{
Handle<String> SSID;
Handle<String> Password;
}WifiNetwork;


//========
// Status
//========

enum class WifiStatus
{
Offline,
Online,
Reset
};


//==============
// Wifi-Adapter
//==============

class WifiAdapter: public Object
{
public:
	// Using
	using Task=Concurrency::Task;

	// Con-/Destructors
	~WifiAdapter();

	// Common
	WifiNetwork AccessPoint;
	Handle<Task> ConnectAsync();
	static Handle<WifiAdapter> Get();
	WifiNetwork Station;

private:
	// Using
	using EmmcHost=Devices::Emmc::EmmcHost;
	using EmmcStatus=Devices::Emmc::EmmcStatus;

	// Con-/Destructors
	WifiAdapter();

	// Common
	VOID DoConnect();
	VOID DoReset();
	Concurrency::Mutex m_Mutex;
	Handle<EmmcHost> m_EmmcHost;
	WifiStatus m_Status;
	static WifiAdapter* s_Current;
};

}}