//========
// Emmc.h
//========

#pragma once


//=======
// Using
//=======

#include "Devices/IoHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Emmc {


//===========
// Registers
//===========

typedef struct
{
RW32 ARG2;
RW32 BLKSIZECNT;
RW32 ARG1;
RW32 CMD;
RW32 RESP[4];
RW32 DATA;
RW32 STATUS;
RW32 CTRL0;
RW32 CTRL1;
RW32 IRQ;
RW32 IRQ_MASK;
RW32 IRQ_EN;
RW32 CTRL2;
RW32 RSVD0[12];
RW32 BOOT_TIMEOUT;
RW32 RSVD1[4];
RW32 EXRDFIFO_EN;
RW32 RSVD2[26];
RW32 SPI_IRQ_SPT;
RW32 RSVD3[2];
RW32 SLOTISR_VER;
}EMMC_REGS;

constexpr BITS BLKSIZECNT_COUNT			={ 0xFFFF, 16 };
constexpr BITS BLKSIZECNT_SIZE			={ 0x3FF, 0 };

constexpr UINT CTRL0_SPI_MODE_EN		=(1<<20);
constexpr BITS CTRL0_POWER				={ 0xF, 8 };
constexpr UINT CTRL0_POWER_OFF			=0;
constexpr UINT CTRL0_POWER_VDD1			=0xF;
constexpr UINT CTRL0_HCTL_8BIT			=(1<<5);
constexpr UINT CTRL0_HCTL_HS_EN			=(1<<2);
constexpr UINT CTRL0_HCTL_DWIDTH4		=(1<<1);

constexpr UINT CTRL1_RESET_DATA			=(1<<26);
constexpr UINT CTRL1_RESET_CMD			=(1<<25);
constexpr UINT CTRL1_RESET_HOST			=(1<<24);
constexpr UINT CTRL1_RESET_ALL			=(0x7<<24);
constexpr BITS CTRL1_DATA_DTO			={ 0xF, 16 };
constexpr UINT CTRL1_DATA_DTO_DEFAULT	=0xE;
constexpr BITS CTRL1_CLK_FREQ_LO		={ 0xFF, 8 };
constexpr BITS CTRL1_CLK_FREQ_HI		={ 0x3, 6 };
constexpr UINT CTRL1_CLK_GENSEL			=(1<<5);
constexpr UINT CTRL1_CLK_EN				=(1<<2);
constexpr UINT CTRL1_CLK_STABLE			=(1<<1);
constexpr UINT CTRL1_CLK_INTLEN			=(1<<0);

constexpr BITS CTRL2_UHSMODE			={ 0x7, 16 };
constexpr UINT CTRL2_UHSMODE_SDR12		=0;
constexpr UINT CTRL2_UHSMODE_SDR25		=1;
constexpr UINT CTRL2_UHSMODE_SDR50		=2;
constexpr UINT CTRL2_UHSMODE_SDR104		=3;
constexpr UINT CTRL2_UHSMODE_DDR50		=4;

constexpr UINT IRQF_ERR_AUTO			=(1<<24);
constexpr UINT IRQF_ERR_DATA_END		=(1<<22);
constexpr UINT IRQF_ERR_DATA_CRC		=(1<<21);
constexpr UINT IRQF_ERR_DATA_TIMEOUT	=(1<<20);
constexpr UINT IRQF_ERR_INDEX			=(1<<19);
constexpr UINT IRQF_ERR_END				=(1<<18);
constexpr UINT IRQF_ERR_CRC				=(1<<17);
constexpr UINT IRQF_ERR_TIMEOUT			=(1<<16);
constexpr UINT IRQF_ERR					=(1<<15);
constexpr UINT IRQF_ENDBOOT				=(1<<14);
constexpr UINT IRQF_BOOTACK				=(1<<13);
constexpr UINT IRQF_RETUNE				=(1<<12);
constexpr UINT IRQF_CARD				=(1<<8);
constexpr UINT IRQF_READ_RDY			=(1<<5);
constexpr UINT IRQF_WRITE_RDY			=(1<<4);
constexpr UINT IRQF_BLOCK_GAP			=(1<<2);
constexpr UINT IRQF_DATA_DONE			=(1<<1);
constexpr UINT IRQF_CMD_DONE			=(1<<0);
constexpr UINT IRQF_DEFAULT				=(IRQF_ERR|IRQF_READ_RDY|IRQF_WRITE_RDY|IRQF_DATA_DONE|IRQF_CMD_DONE);

constexpr BITS STATUS_LEVEL_1			={ 0xF, 25 };
constexpr UINT STATUS_CMD_LEVEL			=(1<<24);
constexpr BITS STATUS_LEVEL_0			={ 0xF, 20 };
constexpr UINT STATUS_WRITE_PROT		=(1<<19);
constexpr UINT STATUS_CARD_INSERTED		=(1<<16);
constexpr UINT STATUS_READ_AVAILABLE	=(1<<11);
constexpr UINT STATUS_WRITE_AVAILABLE	=(1<<10);
constexpr UINT STATUS_READ_TRANSFER		=(1<<9);
constexpr UINT STATUS_WRITE_TRANSFER	=(1<<8);
constexpr UINT STATUS_DAT_ACTIVE		=(1<<2);
constexpr UINT STATUS_DAT_INHIBIT		=(1<<1);
constexpr UINT STATUS_CMD_INHIBIT		=(1<<0);

constexpr BITS VER_HOST_SPEC			={ 0xFF, 16 };
constexpr UINT VER_HOST_SPEC_V1			=0;
constexpr UINT VER_HOST_SPEC_V2			=1;
constexpr UINT VER_HOST_SPEC_V3			=2;


//===============
// Command-Flags
//===============

constexpr UINT CMF_TYPE_NORMAL			=(0<<22);
constexpr UINT CMF_TYPE_SUSPEND			=(1<<22);
constexpr UINT CMF_TYPE_RESUME			=(2<<22);
constexpr UINT CMF_TYPE_ABORT			=(3<<22);
constexpr UINT CMF_ISDATA				=(1<<21);
constexpr UINT CMF_IXCHK				=(1<<20);
constexpr UINT CMF_CRCCHK				=(1<<19);
constexpr UINT CMF_RSPNS_MASK			=(3<<16);
constexpr UINT CMF_RSPNS_136			=(1<<16);
constexpr UINT CMF_RSPNS_48				=(2<<16);
constexpr UINT CMF_RSPNS_48_BUSY		=(3<<16);
constexpr UINT CMF_MULTI_BLOCK			=(1<<5);
constexpr UINT CMF_DAT_DIR_READ			=(1<<4);
constexpr UINT CMF_DAT_DIR_WRITE		=(0<<4);
constexpr UINT CMF_AUTO_CMD_23			=(1<<3);
constexpr UINT CMF_AUTO_CMD_12			=(1<<2);
constexpr UINT CMF_BLKCNT_EN			=(1<<1);


//==========
// Commands
//==========

enum class EmmcCmd: UINT
{
GoIdle				=(0<<24),
SendCid				=(2<<24)|CMF_RSPNS_136|CMF_CRCCHK,
SendRelAddr			=(3<<24)|CMF_RSPNS_48|CMF_CRCCHK,
SendOpCond			=(5<<24)|CMF_RSPNS_48,
SelectCard			=(7<<24)|CMF_RSPNS_48_BUSY|CMF_CRCCHK,
SendIfCond			=(8<<24)|CMF_RSPNS_48|CMF_CRCCHK,
SendCsd				=(9<<24)|CMF_RSPNS_136|CMF_CRCCHK,
ReadSingle			=(17<<24)|CMF_RSPNS_48|CMF_CRCCHK|CMF_ISDATA|CMF_DAT_DIR_READ,
ReadMulti			=(18<<24)|CMF_RSPNS_48|CMF_CRCCHK|CMF_ISDATA|CMF_DAT_DIR_READ|CMF_MULTI_BLOCK|CMF_BLKCNT_EN|CMF_AUTO_CMD_12,
WriteSingle			=(24<<24)|CMF_RSPNS_48|CMF_CRCCHK|CMF_ISDATA|CMF_DAT_DIR_WRITE,
WriteMulti			=(25<<24)|CMF_RSPNS_48|CMF_CRCCHK|CMF_ISDATA|CMF_DAT_DIR_WRITE|CMF_MULTI_BLOCK|CMF_BLKCNT_EN|CMF_AUTO_CMD_12,
IoRwDirect			=(52<<24)|CMF_RSPNS_48,
IoReadSingle		=(53<<24)|CMF_RSPNS_48|CMF_ISDATA|CMF_DAT_DIR_READ,
IoReadMulti			=(53<<24)|CMF_RSPNS_48|CMF_ISDATA|CMF_DAT_DIR_READ|CMF_MULTI_BLOCK|CMF_BLKCNT_EN,
IoWriteSingle		=(53<<24)|CMF_RSPNS_48|CMF_ISDATA|CMF_DAT_DIR_WRITE,
IoWriteMulti		=(53<<24)|CMF_RSPNS_48|CMF_ISDATA|CMF_DAT_DIR_WRITE|CMF_MULTI_BLOCK|CMF_BLKCNT_EN,
AppCmd				=(55<<24)|CMF_RSPNS_48|CMF_CRCCHK
};

constexpr UINT IF_COND_DEFAULT		=0x1AA;

constexpr UINT IO_RW_WRITE			=(1<<31);
constexpr UINT IO_RW_READ			=(0<<31);
constexpr BITS IO_RW_FUNC			={ 0x7, 28 };
constexpr UINT IO_RW_BLK			=(1<<27);
constexpr UINT IO_RW_INCR			=(1<<26);
constexpr BITS IO_RW_ADDR			={ 0x1FFFF, 9};
constexpr UINT IO_RW_ADDR_MAX		=0x1FFFF;
constexpr BITS IO_RW_COUNT			={ 0x1FF, 0};
constexpr UINT IO_RW_COUNT_MAX		=256;
constexpr BITS IO_RW_DATA			={ 0xFF, 0};

enum class IoRwFlags: UINT
{
Read=IO_RW_READ,
ReadIncr=IO_RW_READ|IO_RW_INCR,
Write=IO_RW_WRITE,
WriteIncr=IO_RW_WRITE|IO_RW_INCR
};

constexpr UINT OP_COND_3V3			=(3<<20);
constexpr UINT OP_COND_SUCCESS		=(1<<31);

constexpr BITS RELADDR_RCA			={ 0xFFFF, 16 };


//==============
// App-Commands
//==============

enum class EmmcAppCmd: UINT
{
SendOpCond=(41<<24)|CMF_RSPNS_48
};


//==========
// Function
//==========

typedef struct
{
WORD ID;
WORD BLOCK_SIZE;
}EMMC_FN;


//==========
// Register
//==========

typedef struct
{
EMMC_FN FN;
UINT ADDR;
}EMMC_REG;

}}