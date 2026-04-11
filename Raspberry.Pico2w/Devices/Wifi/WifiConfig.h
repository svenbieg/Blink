//==============
// WifiConfig.h
//==============

#pragma once


//=======
// Using
//=======

#include "Devices/Gpio/GpioHost.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==========
// Settings
//==========

const UINT WIFI_STACK_SIZE		=512;


//======================
// Forward-Declarations
//======================

using GpioPin=Devices::Gpio::GpioPin;


//======
// Pins
//======

const GpioPin PIN_CHIPSEL	=GpioPin::Gpio25;
const GpioPin PIN_CLOCK		=GpioPin::Gpio29;
const GpioPin PIN_DATA		=GpioPin::Gpio24;
const GpioPin PIN_POWER		=GpioPin::Gpio23;


//===========
// Backplane
//===========

const UINT SB_BLOCK_SIZE		=64;
const BYTE SB_DELAY_BYTES		=16;
const BYTE SB_DELAY_COUNT		=SB_DELAY_BYTES/4;


//======
// Wifi
//======

const UINT32 WIFI_BLOCK_SIZE	=512;
const UINT64 WIFI_MAC_ADDR		=0x5E59B550A000ULL;

const UINT WIFI_WRAP			=0x100000;
const UINT WIFI_SDIO_BASE		=0x18002000;
const UINT WIFI_ARM_BASE		=0x18003000;
const UINT WIFI_SRAM_BASE		=0x18004000;

const UINT ARM_RAM_BASE			=0;
const UINT ARM_RAM_SIZE			=512*1024;

}}