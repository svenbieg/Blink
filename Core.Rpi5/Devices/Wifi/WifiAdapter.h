//===============
// WifiAdapter.h
//===============

#pragma once


//=======
// Using
//=======

#include "Devices/Emmc/EmmcDevice.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


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
	using Mutex=Concurrency::Mutex;
	using Task=Concurrency::Task;

	// Common
	Handle<Task> ConnectAsync();
	Event<WifiAdapter> Connected;
	Event<WifiAdapter> ConnectionFailed;
	static Handle<WifiAdapter> Open();

private:
	// Using
	using EmmcDevice=Devices::Emmc::EmmcDevice;

	// Con-/Destructors
	WifiAdapter();

	// Common
	VOID DoConnect();
	VOID DoInitialize();
	Mutex m_Mutex;
	Handle<EmmcDevice> m_EmmcDevice;
	WifiStatus m_Status;
	static Handle<WifiAdapter> s_Current;
};

}}
