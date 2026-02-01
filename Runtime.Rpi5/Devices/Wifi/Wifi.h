//========
// Wifi.h
//========

#pragma once


//=======
// Using
//=======

#include "Devices/Emmc/Emmc.h"
#include "BitHelper.h"

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

const EMMC_FN FN0={ 0, 64 };

const EMMC_REG CCCR_IOENABLE			={ FN0, 0x02 };
const EMMC_REG CCCR_IOREADY				={ FN0, 0x03 };
const EMMC_REG CCCR_INT_EN				={ FN0, 0x04 };
const EMMC_REG CCCR_INT_PENDING			={ FN0, 0x05 };
const EMMC_REG CCCR_BUS_IFC				={ FN0, 0x07 };
const EMMC_REG CCCR_BLKSIZE_FN0			={ FN0, 0x10 };
const EMMC_REG CCCR_SPEED_CTRL			={ FN0, 0x13 };
const EMMC_REG CCCR_BLKSIZE_FN1			={ FN0, 0x110 };
const EMMC_REG CCCR_BLKSIZE_FN2_0		={ FN0, 0x210 };
const EMMC_REG CCCR_BLKSIZE_FN2_1		={ FN0, 0x211 };

const UINT BUS_IFC_32BIT				=2;

const UINT FN0_BIT						=(1<<0);
const UINT FN1_BIT						=(1<<1);
const UINT FN2_BIT						=(1<<2);

const UINT SPEED_CTRL_EHS				=2;


//===========
// Backplane
//===========

const EMMC_FN FN1={ 1, 64 };

const EMMC_REG SB_WINDOW_0				={ FN1, 0x1000A };
const EMMC_REG SB_WINDOW_1				={ FN1, 0x1000B };
const EMMC_REG SB_WINDOW_2				={ FN1, 0x1000C };
const EMMC_REG SB_FRAME_CTRL			={ FN1, 0x1000D };
const EMMC_REG SB_CLK_CSR				={ FN1, 0x1000E };
const EMMC_REG SB_PULLUPS				={ FN1, 0x1000F };
const EMMC_REG SB_WFRM_CNT				={ FN1, 0x10019 };
const EMMC_REG SB_RFRM_CNT_0			={ FN1, 0x1001B };
const EMMC_REG SB_RFRM_CNT_1			={ FN1, 0x1001C };

const BYTE CLK_CSR_HT_AVAIL				=(1<<7);
const BYTE CLK_CSR_FORCE_HT				=(1<<1);

const BYTE FRAME_CTRL_RFHALT			=(1<<0);
const BYTE FRAME_CTRL_WFHALT			=(1<<1);

const UINT SB_ADDR_32BIT				=(1<<15);
const UINT SB_WND_SIZE					=0x8000;
const UINT SB_WND_MASK					=0x7FFF;


//========
// Common
//========

const UINT COM_BASE				=0x18000000;

const UINT IOCTRL				=0x408;
const UINT IOCTRL_FGC			=(1<<1);
const UINT IOCTRL_CLOCK			=(1<<0);
const UINT RESET				=0x800;
const UINT RESET_BIT			=(1<<0);


//=====
// Arm
//=====

const UINT ARM_BASE					=0x18102000;

const UINT ARM_IOCTRL_HALT			=(1<<5);
const UINT ARM_RAM_BASE				=0x198000;
const UINT ARM_RAM_SIZE				=0xC8000;
const UINT ARM_RESET_VECTOR			=0;


//======
// Sdio
//======

const UINT SDIO_BASE				=0x18004000;
const UINT SDIO_INT_STATUS			=SDIO_BASE+0x20;
const UINT SDIO_INT_MASK			=SDIO_BASE+0x24;
const UINT SDIO_MBOX				=SDIO_BASE+0x40;
const UINT SDIO_MBOX_DATA_OUT		=SDIO_BASE+0x48;
const UINT SDIO_MBOX_DATA_IN		=SDIO_BASE+0x4C;

const UINT INT_MAILBOX				=(1<<7);
const UINT INT_FRAME				=(1<<6);
const UINT INT_FCCHANGE				=(1<<5);
const UINT INT_FCSTATE				=(1<<4);
const UINT INT_HOST					=(0xF<<4);

const UINT MBOX_DATA_VER			=(4<<16);
const UINT MBOX_DATA_HALT			=(1<<4);
const UINT MBOX_DATA_ACK			=(1<<1);


//======
// Wifi
//======

const EMMC_FN FN2={ 2, 512 };

}}