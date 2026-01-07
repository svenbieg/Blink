//==============
// WifiEvents.h
//==============

#pragma once


//=======
// Using
//=======

#include "BitHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//========
// Events
//========

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

consteval BYTE WIFI_EVENT_FLAGS(const UINT Id)
{
BYTE flags=0xFF;
UINT id_min=Id*8;
UINT id_max=(Id+1)*8-1;
UINT count=TypeHelper::ArraySize(WIFI_DISABLED_EVENTS);
for(UINT u=0; u<count; u++)
	{
	UINT id=WIFI_DISABLED_EVENTS[u];
	if(id<id_min||id>id_max)
		continue;
	BYTE mask=1<<(id-id_min);
	BitHelper::Clear(flags, mask);
	}
return flags;
}

constexpr BYTE WIFI_EVENT_MASK[16]=
	{
	WIFI_EVENT_FLAGS(0), WIFI_EVENT_FLAGS(1), WIFI_EVENT_FLAGS(2), WIFI_EVENT_FLAGS(3),
	WIFI_EVENT_FLAGS(4), WIFI_EVENT_FLAGS(5), WIFI_EVENT_FLAGS(6), WIFI_EVENT_FLAGS(7),
	WIFI_EVENT_FLAGS(8), WIFI_EVENT_FLAGS(9), WIFI_EVENT_FLAGS(10), WIFI_EVENT_FLAGS(11),
	WIFI_EVENT_FLAGS(12), WIFI_EVENT_FLAGS(13), WIFI_EVENT_FLAGS(14), WIFI_EVENT_FLAGS(15)
	};

}}