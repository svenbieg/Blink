//========
// Wifi.h
//========

#pragma once


//=======
// Using
//=======

#include "Devices/Sdio/Sdio.h"
#include "Devices/Wifi/WifiConfig.h"
#include "Network/Ethernet/Ethernet.h"
#include <array>


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//======================
// Forward-Declarations
//======================

using SDIO_FN=Devices::Sdio::SDIO_FN;
using SDIO_REG=Devices::Sdio::SDIO_REG;

using FN0_08=Devices::Sdio::FN0_08;
using FN0_16=Devices::Sdio::FN0_16;
using FN0_32=Devices::Sdio::FN0_32;
using FN1_08=Devices::Sdio::FN1_08;


//===========
// Backplane
//===========

const FN1_08 SB_FN2_WATERMARK	=0x10008;
const FN1_08 SB_WINDOW_0		=0x1000A;
const FN1_08 SB_WINDOW_1		=0x1000B;
const FN1_08 SB_WINDOW_2		=0x1000C;
const FN1_08 SB_FRAME_CTRL		=0x1000D;
const FN1_08 SB_CLK_CSR			=0x1000E;
const FN1_08 SB_PULLUPS			=0x1000F;
const FN1_08 SB_WFRM_CNT		=0x10019;
const FN1_08 SB_RFRM_CNT_0		=0x1001B;
const FN1_08 SB_RFRM_CNT_1		=0x1001C;

const UINT SB_WND_MASK			=0x7FFF;
const UINT SB_WND_SIZE			=0x8000;
const UINT SB_ADDR_32BIT		=0x8000;

const BYTE FN2_SPI_WATERMARK	=(1<<5);

const BYTE CLK_CSR_HT_AVAIL		=(1<<7);
const BYTE CLK_CSR_ALP_AVAIL	=(1<<6);
const BYTE CLK_CSR_HT_REQ		=(1<<4);
const BYTE CLK_CSR_ALP_REQ		=(1<<3);
const BYTE CLK_CSR_HT_FORCE		=(1<<1);

const BYTE FRAME_CTRL_RFHALT	=(1<<0);
const BYTE FRAME_CTRL_WFHALT	=(1<<1);


//========
// Common
//========

const UINT SB_COM_BASE			=0x18000000;

const UINT COM_IOCTRL			=0x408;
const UINT IOCTRL_FGC			=(1<<1);
const UINT IOCTRL_CLOCK			=(1<<0);

const UINT COM_RESET			=0x800;
const UINT RESET_BIT			=(1<<0);


//=====
// ARM
//=====

const UINT ARM_IOCTRL_HALT		=(1<<5);
const UINT ARM_RESET_VECTOR		=0;


//======
// SDIO
//======

const UINT SDIO_INT_STATUS		=WIFI_SDIO_BASE+0x20;
const UINT SDIO_INT_MASK		=WIFI_SDIO_BASE+0x24;
const UINT SDIO_MBOX			=WIFI_SDIO_BASE+0x40;
const UINT SDIO_MBOX_DATA_OUT	=WIFI_SDIO_BASE+0x48;
const UINT SDIO_MBOX_DATA_IN	=WIFI_SDIO_BASE+0x4C;

const UINT SDIO_INT_MAILBOX		=(1<<7);
const UINT SDIO_INT_FRAME		=(1<<6);
const UINT SDIO_INT_FCCHANGE	=(1<<5);
const UINT SDIO_INT_FCSTATE		=(1<<4);
const UINT SDIO_INT_HOST_MASK	=SDIO_INT_MAILBOX|SDIO_INT_FRAME|SDIO_INT_FCCHANGE|SDIO_INT_FCSTATE;

const UINT MBOX_DATA_VER		=(4<<16);
const UINT MBOX_DATA_HALT		=(1<<4);
const UINT MBOX_DATA_ACK		=(1<<1);


//======
// SRAM
//======

const UINT SRAM_BANKX_INDEX		=WIFI_SRAM_BASE+0x10;
const UINT SRAM_BANKX_PDA		=WIFI_SRAM_BASE+0x44;


//===========
// Interface
//===========

const UINT WIFI_AP=1;
const UINT WIFI_STA=0;


//========
// Header
//========

typedef struct alignas(UINT)
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

typedef struct
{
BYTE Flags;
BYTE Priority;
BYTE Flags2;
BYTE DataOffset;
}WIFI_BDC_HEADER;


//==========
// Commands
//==========

enum class WifiCommand: UINT
{
Up						=2,
SetPromisc				=10,
SetInfra				=20,
SetAuth					=22,
SetSsid					=26,
SetChannel				=30,
SetPassiveScan			=49,
SetAntDiv				=64,
SetDtimPeriod			=78,
SetGMode				=110,
SetWSec					=134,
SetWpaAuth				=165,
SetScanChannelTime		=185,
SetScanUnassocTime		=187,
SetScanPassiveTime		=258,
GetVariable				=262,
SetVariable				=263,
SetWSecPmk				=268
};

enum class WifiCmdFlags: WORD
{
Error			=(1<<0),
Read			=(0<<1),
Write			=(1<<1)
};

const UINT WIFI_CMD_INTERFACE_SHIFT=12;

typedef struct
{
WifiCommand Command;
UINT Length;
WifiCmdFlags Flags;
WORD Id;
UINT Status;
}WIFI_CMD;


//========
// Events
//========

enum class WifiEventType: UINT
{
SetSsid=0,
Join=1,
Auth=3,
Deauth=5,
DeauthInd=6,
Disassoc=12,
Link=16,
PskSup=46,
EscanResult=69,
CsaCompleteInd=80
};

const UINT WIFI_ENABLED_EVENTS[]=
{
static_cast<UINT>(WifiEventType::SetSsid),
//static_cast<UINT>(WifiEventType::Join),
static_cast<UINT>(WifiEventType::Auth),
static_cast<UINT>(WifiEventType::Deauth),
static_cast<UINT>(WifiEventType::DeauthInd),
static_cast<UINT>(WifiEventType::Disassoc),
static_cast<UINT>(WifiEventType::Link),
static_cast<UINT>(WifiEventType::PskSup),
static_cast<UINT>(WifiEventType::EscanResult),
static_cast<UINT>(WifiEventType::CsaCompleteInd)
};

typedef struct
{
UINT Bss;
std::array<BYTE, 19> Flags;
}WIFI_EVENT_MASK;

const UINT WIFI_EVENT_MASK_SIZE=23;

const auto WIFI_EVENT_FLAGS=[]()
{
std::array<BYTE, 19> flags={ 0 };
for(auto id: WIFI_ENABLED_EVENTS)
	flags[id/8]|=static_cast<BYTE>(1U<<(id%8));
return flags;
}();

const WIFI_EVENT_MASK WIFI_EVENT_MASK_AP={ WIFI_AP, WIFI_EVENT_FLAGS };
const WIFI_EVENT_MASK WIFI_EVENT_MASK_STA={ WIFI_STA, WIFI_EVENT_FLAGS };

typedef struct
{
WORD Subtype;
WORD Length;
BYTE Version;
BYTE Oui[3];
WORD UserSubtype;
}WIFI_EVENT_HEADER;

typedef struct
{
WORD Version;
WORD Flags;
WifiEventType EventType;
UINT Status;
UINT Reason;
UINT AuthType;
UINT DataLength;
BYTE Address[6];
CHAR InterfaceName[16];
BYTE InterfaceIndex;
BYTE BssConfigIndex;
}WIFI_EVENT_MESSAGE;

typedef struct
{
Network::Ethernet::ETHERNET_HEADER Ethernet;
WIFI_EVENT_HEADER Broadcom;
WIFI_EVENT_MESSAGE Event;
}WIFI_EVENT_FRAME;


//=========
// Country
//=========

typedef struct
{
CHAR Abbreviation[4];
INT Revision;
CHAR Code[4];
}WIFI_COUNTRY;

consteval auto WIFI_COUNTRY_CODE(LPCSTR Code)
{
WIFI_COUNTRY country={ 0 };
country.Revision=-1;
for(UINT u=0; Code[u]; u++)
	{
	country.Abbreviation[u]=Code[u];
	country.Code[u]=Code[u];
	}
return country;
}

const WIFI_COUNTRY WIFI_COUNTRY_DE=WIFI_COUNTRY_CODE("DE");


//==========
// Security
//==========

typedef struct
{
WORD Length;
WORD Flags;
CHAR Key[64];
}WIFI_WSEC_PMK;

const UINT WIFI_AUTH_WPA2_AES_PSK=0x40004;

}}