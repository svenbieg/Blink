//==============
// WifiPacket.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/PacketBuffer.h"
#include "BitHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==========
// Settings
//==========

static constexpr UINT WIFI_PACKET_MAX=2048;


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

static constexpr BITS8 WIFI_HEADER_FLAGS_TYPE={ 0xF, 0 };

enum class WifiPacketType
{
Response=0,
Event=1,
Data=2
};


//=========
// Command
//=========

enum class WifiCmd: UINT
{
Up						=2,
SetPromisc				=10,
SetInfra				=20,
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