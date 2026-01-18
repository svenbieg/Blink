//============
// WifiScan.h
//============

#pragma once


//=======
// Using
//=======

#include "Devices/Wifi/WifiCommand.h"
#include <array>


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==========
// Channels
//==========

typedef struct
{
BYTE Id;
BYTE Flags;
}WIFI_SCAN_CHANNEL;

constexpr auto WIFI_SCAN_CHANNELS=std::array<WIFI_SCAN_CHANNEL, 14>({
		{ 1, 0x2B }, { 2, 0x2B }, { 3, 0x2B }, { 4, 0x2B }, { 5, 0x2E }, { 6, 0x2E }, { 7, 0x2E },
		{ 8, 0x2B }, { 9, 0x2B }, { 10, 0x2B }, { 11, 0x2B }, { 12, 0x2B }, { 13, 0x2B }, { 14, 0x2B }});


//===========
// Wifi-Scan
//===========

constexpr UINT WIFI_SCAN_VERSION=1;

enum class WifiBssType: BYTE
{
Independent,
Infrastructure,
Any=2
};

enum class WifiScanAction: WORD
{
Start=1,
Continue=2,
Abort=3
};

enum class WifiScanType: BYTE
{
Active=0,
Passive=1
};

typedef struct
{
UINT Version;
WifiScanAction Action;
WORD SyncId;
UINT SsidLength;
CHAR Ssid[32];
std::array<BYTE, 6> BssId;
WifiBssType BssType;
WifiScanType ScanType;
UINT ProbesCount;
UINT ActiveTime;
UINT PassiveTime;
UINT HomeTime;
}WIFI_SCAN_PARAMS;

}}