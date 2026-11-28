//================
// WifiFirmware.h
//================

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//======================
// Forward-Declarations
//======================

class WifiAdapter;


//==========
// Firmware
//==========

typedef struct
{
UINT* Config;
UINT ConfigSize;
UINT* Image;
UINT ImageSize;
}WIFI_FIRMWARE;


//===============
// Wifi-Firmware
//===============

class WifiFirmware
{
public:
	// Common
	static WIFI_FIRMWARE const& Get();
	static VOID UploadRegulatory(WifiAdapter* Wifi);

private:
	// Common
	static WIFI_FIRMWARE s_Firmware;
};

}}