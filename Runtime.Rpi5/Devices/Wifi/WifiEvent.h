//=============
// WifiEvent.h
//=============

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


//=============
// Wifi-Events
//=============

constexpr UINT WIFI_EVENT_TX_FAIL			=20;
constexpr UINT WIFI_EVENT_RADIO				=40;
constexpr UINT WIFI_EVENT_PROBREQ_MSG		=44;
constexpr UINT WIFI_EVENT_IF				=54;
constexpr UINT WIFI_EVENT_PROBRESP_MSG		=71;
constexpr UINT WIFI_EVENT_CCA_CHAN_QUAL		=124;


//============
// Event-Mask
//============

constexpr UINT WIFI_DISABLED_EVENTS[]=
{
WIFI_EVENT_TX_FAIL,
WIFI_EVENT_RADIO,
WIFI_EVENT_PROBREQ_MSG,
WIFI_EVENT_IF,
WIFI_EVENT_PROBRESP_MSG,
WIFI_EVENT_CCA_CHAN_QUAL
};

constexpr auto WIFI_EVENT_MASK=[]()
{
std::array<BYTE, 16> flags;
flags.fill(0xFF);
for(auto id: WIFI_DISABLED_EVENTS)
	flags[id/8]&=~(1<<(id%8));
return flags;
}();

}}