//=================
// EmmcRegisters.h
//=================

#pragma once


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
REG ARG2;
REG BLKSIZECNT;
REG ARG1;
REG CMDTM;
REG RESP0;
REG RESP1;
REG RESP2;
REG RESP3;
REG DATA;
REG STATUS;
REG CTRL0;
REG CTRL1;
REG IRQ;
REG IRQ_MASK;
REG IRQ_EN;
REG CTRL2;
REG RSVD0[12];
REG BOOT_TIMEOUT;
REG RSVD1[4];
REG EXRDFIFO_EN;
REG RSVD2[26];
REG SPI_IRQ_SPT;
REG RSVD3[2];
REG SLOTISR_VER;
}EMMC_REGS;

namespace CTRL0
{
constexpr UINT			SPI_MODE_EN		=(1<<20);
constexpr UINT			HCTL_8BIT		=(1<<5);
constexpr UINT			HCTL_HS_EN		=(1<<2);
constexpr UINT			HCTL_DWIDTH		=(1<<1);
constexpr BIT_FIELD		POWER			={ 0xF, 8 };
constexpr UINT			POWER_ON		=1;
constexpr UINT			POWER_VDD1		=0xF;
}

namespace CTRL1
{
constexpr UINT			RESET_DATA			=(1<<26);
constexpr UINT			RESET_CMD			=(1<<25);
constexpr UINT			RESET_HOST			=(1<<24);
constexpr UINT			RESET_ALL			=(0x7<<24);
constexpr BIT_FIELD		DATA_DTO			={ 0xF, 16 };
constexpr UINT			DATA_DTO_DEFAULT	=14;
constexpr BIT_FIELD		CLK_FREQ_LO			={ 0xF, 8 };
constexpr BIT_FIELD		CLK_FREQ_HI			={ 0x3, 6 };
constexpr UINT			CLK_GENSEL			=(1<<5);
constexpr UINT			CLK_EN				=(1<<2);
constexpr UINT			CLK_STABLE			=(1<<1);
constexpr UINT			CLK_INTLEN			=(1<<0);
}

namespace CTRL2
{
constexpr BIT_FIELD		UHSMODE				={ 0x7, 16 };
constexpr UINT			UHSUHSMODE_SDR12	=0;
constexpr UINT			UHSUHSMODE_SDR25	=1;
constexpr UINT			UHSUHSMODE_SDR50	=2;
constexpr UINT			UHSUHSMODE_SDR104	=3;
constexpr UINT			UHSUHSMODE_DDR50	=4;
}

namespace IRQ
{
constexpr UINT CMD_DONE			=(1<<0);
constexpr UINT DATA_DONE		=(1<<1);
constexpr UINT BLOCK_GAP		=(1<<2);
constexpr UINT WRITE_RDY		=(1<<4);
constexpr UINT READ_RDY			=(1<<5);
constexpr UINT CARD				=(1<<8);
constexpr UINT RETUNE			=(1<<12);
constexpr UINT BOOTACK			=(1<<13);
constexpr UINT ENDBOOT			=(1<<14);
constexpr UINT ERR				=(1<<15);
constexpr UINT CMD_TIMEOUT		=(1<<16);
constexpr UINT CRC_ERROR		=(1<<17);
constexpr UINT END_ERROR		=(1<<18);
constexpr UINT INDEX_ERROR		=(1<<19);
constexpr UINT DATA_TIMEOUT		=(1<<20);
constexpr UINT DATA_CRC_ERR		=(1<<21);
constexpr UINT DATA_END_ERR		=(1<<22);
constexpr UINT AUTO_ERROR		=(1<<24);
constexpr UINT MASK_ERROR		=(CRC_ERROR|END_ERROR|INDEX_ERROR|DATA_TIMEOUT|DATA_CRC_ERR|DATA_END_ERR|ERR|AUTO_ERROR);
constexpr UINT MASK_ALL			=(CMD_DONE|DATA_DONE|READ_RDY|WRITE_RDY|MASK_ERROR);
}

namespace SLOTISR_VER
{
constexpr BIT_FIELD		HOST_SPEC={ 0xFF, 16 };
constexpr UINT			HOST_SPEC_V1=0;
constexpr UINT			HOST_SPEC_V2=1;
constexpr UINT			HOST_SPEC_V3=2;
}

namespace STATUS
{
constexpr UINT CMD_INHIBIT		=(1<<0);
constexpr UINT DAT_INHIBIT		=(1<<1);
constexpr UINT DAT_ACTIVE		=(1<<2);
constexpr UINT WRITE_TRANSFER	=(1<<8);
constexpr UINT READ_TRANSFER	=(1<<9);
constexpr UINT WRITE_AVAILABLE	=(1<<10);
constexpr UINT READ_AVAILABLE	=(1<<11);
constexpr UINT WRITE_PROT		=(1<<19);
constexpr UINT DAT0				=(1<<20);
constexpr UINT DAT1				=(1<<21);
constexpr UINT DAT2				=(1<<22);
constexpr UINT DAT3				=(1<<23);
constexpr UINT CMD_LEVEL		=(1<<24);
constexpr UINT DAT_LEVEL0		=(DAT0|DAT1|DAT2|DAT3);
constexpr UINT DAT_LEVEL1		=(0xF<<25);
}

}}
