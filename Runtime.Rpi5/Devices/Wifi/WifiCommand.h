//===============
// WifiCommand.h
//===============

#pragma once


//=======
// Using
//=======

#include "Devices/Wifi/WifiPacket.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//===============
// Wifi-Commands
//===============

enum class WifiCmd: UINT
{
Up						=2,
SetPromisc				=10,
SetInfra				=20,
SetPassiveScan			=49,
SetScanChannelTime		=185,
SetScanUnassocTime		=187,
SetScanPassiveTime		=258,
GetVariable				=262,
SetVariable				=263
};

enum class WifiCmdFlags: WORD
{
Error	=(1<<0),
Read	=(0<<1),
Write	=(1<<1)
};

typedef struct
{
WifiCmd Command;
UINT Length;
WORD Id;
WifiCmdFlags Flags;
UINT Status;
}WIFI_CMD;

}}