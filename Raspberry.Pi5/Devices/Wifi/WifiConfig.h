//==============
// WifiConfig.h
//==============

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


//===============
// Configuration
//===============

const UINT64 WIFI_MAC_ADDR		=0x6CF274EB27B8ULL;

const UINT WIFI_WRAP			=0x100000;
const UINT WIFI_ARM_BASE		=0x18002000;
const UINT WIFI_SDIO_BASE		=0x18004000;
const UINT WIFI_SRAM_BASE		=0;

const UINT ARM_RAM_BASE			=0x198000;
const UINT ARM_RAM_SIZE			=0xC8000;

}}