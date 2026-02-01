//==============
// WifiPacket.h
//==============

#pragma once


//=======
// Using
//=======

#include "Devices/Wifi/Wifi.h"
#include "Storage/PacketBuffer.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==========
// Settings
//==========

static const UINT WIFI_PACKET_MAX=2048;


//========
// Header
//========

typedef struct
{
WORD Length;
WORD LengthChk;
BYTE Sequence;
BYTE Flags;
BYTE NextLength;
BYTE DataOffset;
BYTE FlowControl;
BYTE Window;
WORD Reserved;
}WIFI_HEADER;

static const BITS8 WIFI_HEADER_FLAGS_TYPE={ 0xF, 0 };

enum class WifiPacketType
{
Response=0,
Event=1,
Data=2
};


//=============
// Wifi-Packet
//=============

class WifiPacket: public Storage::PacketBuffer
{
public:
	// Con-/Destructors
	static Handle<WifiPacket> Create(UINT Size);
	static Handle<WifiPacket> Create(WIFI_HEADER const& Header);

	// Common
	BYTE GetSequenceId()const;
	WifiPacketType GetType()const;

private:
	// Con-/Destructors
	friend Object;
	WifiPacket(BYTE* Buffer, SIZE_T Size);

	// Common
	static BYTE s_SequenceId;
};

}}