//========
// Wifi.h
//========

#pragma once


//=======
// Using
//=======

#include "Devices/Sdio/Sdio.h"
#include "Devices/Wifi/WifiConfig.h"
#include "Devices/Wifi/WifiPacket.h"
#include "Network/Ethernet/MacAddress.h"
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

using MAC_ADDR=Network::Ethernet::MAC_ADDR;
const UINT MAC_ADDR_SIZE=Network::Ethernet::MAC_ADDR_SIZE;


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

const UINT INT_MAILBOX			=(1<<7);
const UINT INT_FRAME			=(1<<6);
const UINT INT_FCCHANGE			=(1<<5);
const UINT INT_FCSTATE			=(1<<4);
const UINT INT_HOST				=INT_MAILBOX|INT_FRAME|INT_FCCHANGE|INT_FCSTATE;

const UINT MBOX_DATA_VER		=(4<<16);
const UINT MBOX_DATA_HALT		=(1<<4);
const UINT MBOX_DATA_ACK		=(1<<1);


//======
// SRAM
//======

const UINT SRAM_BANKX_INDEX		=WIFI_SRAM_BASE+0x10;
const UINT SRAM_BANKX_PDA		=WIFI_SRAM_BASE+0x44;


//==========
// Commands
//==========

enum class WifiCommand: UINT
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
WifiCommand Command;
UINT Length;
WORD Id;
WifiCmdFlags Flags;
UINT Status;
}WIFI_CMD;


//========
// Events
//========

const UINT WIFI_EVENT_TX_FAIL			=20;
const UINT WIFI_EVENT_RADIO				=40;
const UINT WIFI_EVENT_PROBREQ_MSG		=44;
const UINT WIFI_EVENT_IF				=54;
const UINT WIFI_EVENT_PROBRESP_MSG		=71;
const UINT WIFI_EVENT_CCA_CHAN_QUAL		=124;

const UINT WIFI_DISABLED_EVENTS[]=
{
WIFI_EVENT_TX_FAIL,
WIFI_EVENT_RADIO,
WIFI_EVENT_PROBREQ_MSG,
WIFI_EVENT_IF,
WIFI_EVENT_PROBRESP_MSG,
WIFI_EVENT_CCA_CHAN_QUAL
};

const auto WIFI_EVENT_MASK=[]()
{
std::array<BYTE, 16> flags;
flags.fill(0xFF);
for(auto id: WIFI_DISABLED_EVENTS)
	flags[id/8]&=~(1<<(id%8));
return flags;
}();

}}