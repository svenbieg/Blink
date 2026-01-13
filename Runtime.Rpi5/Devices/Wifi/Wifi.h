//========
// Wifi.h
//========

#pragma once


//=======
// Using
//=======

#include "Devices/Emmc/Emmc.h"
#include "BitHelper.h"
#include <array>

using EMMC_FN=Devices::Emmc::EMMC_FN;
using EMMC_REG=Devices::Emmc::EMMC_REG;


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//======
// Emmc
//======

constexpr EMMC_FN FN0						={ 0, 64 };

constexpr EMMC_REG CCCR_IOENABLE			={ FN0, 0x02 };
constexpr EMMC_REG CCCR_IOREADY				={ FN0, 0x03 };
constexpr EMMC_REG CCCR_INT_EN				={ FN0, 0x04 };
constexpr EMMC_REG CCCR_INT_PENDING			={ FN0, 0x05 };
constexpr EMMC_REG CCCR_BUS_IFC				={ FN0, 0x07 };
constexpr EMMC_REG CCCR_BLKSIZE_FN0			={ FN0, 0x10 };
constexpr EMMC_REG CCCR_SPEED_CTRL			={ FN0, 0x13 };
constexpr EMMC_REG CCCR_BLKSIZE_FN1			={ FN0, 0x110 };
constexpr EMMC_REG CCCR_BLKSIZE_FN2_0		={ FN0, 0x210 };
constexpr EMMC_REG CCCR_BLKSIZE_FN2_1		={ FN0, 0x211 };

constexpr UINT BUS_IFC_32BIT				=2;

constexpr UINT FN0_BIT						=(1<<0);
constexpr UINT FN1_BIT						=(1<<1);
constexpr UINT FN2_BIT						=(1<<2);

constexpr UINT SPEED_CTRL_EHS				=2;


//===========
// Backplane
//===========

constexpr EMMC_FN FN1						={ 1, 64 };

constexpr EMMC_REG SB_WINDOW_0				={ FN1, 0x1000A };
constexpr EMMC_REG SB_WINDOW_1				={ FN1, 0x1000B };
constexpr EMMC_REG SB_WINDOW_2				={ FN1, 0x1000C };
constexpr EMMC_REG SB_FRAME_CTRL			={ FN1, 0x1000D };
constexpr EMMC_REG SB_CLK_CSR				={ FN1, 0x1000E };
constexpr EMMC_REG SB_PULLUPS				={ FN1, 0x1000F };
constexpr EMMC_REG SB_WFRM_CNT				={ FN1, 0x10019 };
constexpr EMMC_REG SB_RFRM_CNT_0			={ FN1, 0x1001B };
constexpr EMMC_REG SB_RFRM_CNT_1			={ FN1, 0x1001C };

constexpr BYTE CLK_CSR_HT_AVAIL				=(1<<7);
constexpr BYTE CLK_CSR_FORCE_HT				=(1<<1);

constexpr BYTE FRAME_CTRL_RFHALT			=(1<<0);
constexpr BYTE FRAME_CTRL_WFHALT			=(1<<1);

constexpr UINT SB_ADDR_32BIT				=(1<<15);
constexpr UINT SB_WND_SIZE					=0x8000;
constexpr UINT SB_WND_MASK					=0x7FFF;


//========
// Common
//========

constexpr UINT COM_BASE					=0x18000000;

constexpr UINT IOCTRL					=0x408;
constexpr UINT IOCTRL_FGC				=(1<<1);
constexpr UINT IOCTRL_CLOCK				=(1<<0);
constexpr UINT RESET					=0x800;
constexpr UINT RESET_BIT				=(1<<0);


//=====
// Arm
//=====

constexpr UINT ARM_BASE					=0x18102000;
constexpr UINT ARM_IOCTRL_HALT			=(1<<5);
constexpr UINT ARM_RAM_BASE				=0x198000;
constexpr UINT ARM_RAM_SIZE				=0xC8000;
constexpr UINT ARM_RESET_VECTOR			=0;


//======
// Sdio
//======

constexpr UINT SDIO_BASE				=0x18004000;
constexpr UINT SDIO_INT_STATUS			=SDIO_BASE+0x20;
constexpr UINT SDIO_INT_MASK			=SDIO_BASE+0x24;
constexpr UINT SDIO_MBOX				=SDIO_BASE+0x40;
constexpr UINT SDIO_MBOX_DATA_OUT		=SDIO_BASE+0x48;
constexpr UINT SDIO_MBOX_DATA_IN		=SDIO_BASE+0x4C;

constexpr UINT INT_MAILBOX				=(1<<7);
constexpr UINT INT_FRAME				=(1<<6);
constexpr UINT INT_FCCHANGE				=(1<<5);
constexpr UINT INT_FCSTATE				=(1<<4);
constexpr UINT INT_HOST					=(0xF<<4);

constexpr UINT MBOX_DATA_VER			=(4<<16);
constexpr UINT MBOX_DATA_HALT			=(1<<4);
constexpr UINT MBOX_DATA_ACK			=(1<<1);


//======
// Wifi
//======

constexpr EMMC_FN FN2					={ 2, 512 };

typedef struct
{
WORD Flags;
WORD Type;
UINT Size;
UINT Crc;
}CLM_HEADER;

constexpr WORD CLM_TYPE=2;
constexpr UINT CLM_DATA_MAX=1024;

constexpr WORD CLMF_CLM=(1<<12);
constexpr WORD CLMF_LAST=(1<<2);
constexpr WORD CLMF_FIRST=(1<<1);


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

constexpr auto WIFI_EVENT_MASK=[]()
{
std::array<BYTE, 16> flags;
flags.fill(0xFF);
for(auto id: WIFI_DISABLED_EVENTS)
	flags[id/8]&=~(1<<(id%8));
return flags;
}();


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


//======
// Scan
//======

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