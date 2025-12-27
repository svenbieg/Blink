//========
// Emmc.h
//========

#pragma once


//=======
// Using
//=======

#include <io.h>


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
rw32_t ARG2;
rw32_t BLKSIZECNT;
rw32_t ARG1;
rw32_t CMD;
rw32_t RESP[4];
rw32_t DATA;
rw32_t STATUS;
rw32_t CTRL0;
rw32_t CTRL1;
rw32_t IRQ;
rw32_t IRQ_MASK;
rw32_t IRQ_EN;
rw32_t CTRL2;
rw32_t RSVD0[12];
rw32_t BOOT_TIMEOUT;
rw32_t RSVD1[4];
rw32_t EXRDFIFO_EN;
rw32_t RSVD2[26];
rw32_t SPI_IRQ_SPT;
rw32_t RSVD3[2];
rw32_t SLOTISR_VER;
}emmc_regs_t;

// BLKSIZECNT
constexpr bits32_t BLKSIZECNT_COUNT		={ 0xFFFF, 16 };
constexpr bits32_t BLKSIZECNT_SIZE		={ 0x3FF, 0 };

// CTRL0
constexpr uint32_t CTRL0_SPI_MODE_EN	=(1<<20);
constexpr bits32_t CTRL0_POWER			={ 0xF, 8 };
constexpr uint32_t CTRL0_POWER_OFF		=0;
constexpr uint32_t CTRL0_POWER_VDD1		=0xF;
constexpr uint32_t CTRL0_HCTL_8BIT		=(1<<5);
constexpr uint32_t CTRL0_HCTL_HS_EN		=(1<<2);
constexpr uint32_t CTRL0_HCTL_DWIDTH4	=(1<<1);

// CTRL1
constexpr uint32_t CTRL1_RESET_DATA			=(1<<26);
constexpr uint32_t CTRL1_RESET_CMD			=(1<<25);
constexpr uint32_t CTRL1_RESET_HOST			=(1<<24);
constexpr uint32_t CTRL1_RESET_ALL			=(0x7<<24);
constexpr bits32_t CTRL1_DATA_DTO			={ 0xF, 16 };
constexpr uint32_t CTRL1_DATA_DTO_DEFAULT	=0xE;
constexpr bits32_t CTRL1_CLK_FREQ_LO		={ 0xFF, 8 };
constexpr bits32_t CTRL1_CLK_FREQ_HI		={ 0x3, 6 };
constexpr uint32_t CTRL1_CLK_GENSEL			=(1<<5);
constexpr uint32_t CTRL1_CLK_EN				=(1<<2);
constexpr uint32_t CTRL1_CLK_STABLE			=(1<<1);
constexpr uint32_t CTRL1_CLK_INTLEN			=(1<<0);

// CTRL2
constexpr bits32_t CTRL2_UHSMODE			={ 0x7, 16 };
constexpr uint32_t CTRL2_UHSMODE_SDR12		=0;
constexpr uint32_t CTRL2_UHSMODE_SDR25		=1;
constexpr uint32_t CTRL2_UHSMODE_SDR50		=2;
constexpr uint32_t CTRL2_UHSMODE_SDR104		=3;
constexpr uint32_t CTRL2_UHSMODE_DDR50		=4;

// IRQ
constexpr uint32_t IRQF_CMD_DONE			=(1<<0);
constexpr uint32_t IRQF_DATA_DONE			=(1<<1);
constexpr uint32_t IRQF_BLOCK_GAP			=(1<<2);
constexpr uint32_t IRQF_WRITE_RDY			=(1<<4);
constexpr uint32_t IRQF_READ_RDY			=(1<<5);
constexpr uint32_t IRQF_CARD				=(1<<8);
constexpr uint32_t IRQF_RETUNE				=(1<<12);
constexpr uint32_t IRQF_BOOTACK				=(1<<13);
constexpr uint32_t IRQF_ENDBOOT				=(1<<14);
constexpr uint32_t IRQF_ERR					=(1<<15);
constexpr uint32_t IRQF_ERR_TIMEOUT			=(1<<16);
constexpr uint32_t IRQF_ERR_CRC				=(1<<17);
constexpr uint32_t IRQF_ERR_END				=(1<<18);
constexpr uint32_t IRQF_ERR_INDEX			=(1<<19);
constexpr uint32_t IRQF_ERR_DATA_TIMEOUT	=(1<<20);
constexpr uint32_t IRQF_ERR_DATA_CRC		=(1<<21);
constexpr uint32_t IRQF_ERR_DATA_END		=(1<<22);
constexpr uint32_t IRQF_ERR_AUTO			=(1<<24);
constexpr uint32_t IRQF_MASK_ERROR			=(IRQF_ERR_CRC|IRQF_ERR_END|IRQF_ERR_INDEX|IRQF_ERR_DATA_TIMEOUT|IRQF_ERR_DATA_CRC|IRQF_ERR_DATA_END); // (0x7E0000)
constexpr uint32_t IRQF_MASK_DEFAULT		=(IRQF_CMD_DONE|IRQF_DATA_DONE|IRQF_BLOCK_GAP|IRQF_WRITE_RDY|IRQF_READ_RDY|IRQF_MASK_ERROR); // (0x7E0037)

// SLOTISR_VER
constexpr bits32_t SLOTISR_VER_HOST_SPEC		={ 0xFF, 16 };
constexpr uint32_t SLOTISR_VER_HOST_SPEC_V1		=0;
constexpr uint32_t SLOTISR_VER_HOST_SPEC_V2		=1;
constexpr uint32_t SLOTISR_VER_HOST_SPEC_V3		=2;

// STATUS
constexpr uint32_t STATUS_CMD_INHIBIT		=(1<<0);
constexpr uint32_t STATUS_DAT_INHIBIT		=(1<<1);
constexpr uint32_t STATUS_DAT_ACTIVE		=(1<<2);
constexpr uint32_t STATUS_WRITE_TRANSFER	=(1<<8);
constexpr uint32_t STATUS_READ_TRANSFER		=(1<<9);
constexpr uint32_t STATUS_WRITE_AVAILABLE	=(1<<10);
constexpr uint32_t STATUS_READ_AVAILABLE	=(1<<11);
constexpr uint32_t STATUS_CARD_INSERTED		=(1<<16);
constexpr uint32_t STATUS_WRITE_PROT		=(1<<19);
constexpr uint32_t STATUS_DAT0				=(1<<20);
constexpr uint32_t STATUS_DAT1				=(1<<21);
constexpr uint32_t STATUS_DAT2				=(1<<22);
constexpr uint32_t STATUS_DAT3				=(1<<23);
constexpr uint32_t STATUS_CMD_LEVEL			=(1<<24);
constexpr uint32_t STATUS_DAT4				=(1<<25);
constexpr uint32_t STATUS_DAT5				=(1<<26);
constexpr uint32_t STATUS_DAT6				=(1<<27);
constexpr uint32_t STATUS_DAT7				=(1<<28);
constexpr uint32_t STATUS_DAT_LEVEL0		=(0xF<<20);
constexpr uint32_t STATUS_DAT_LEVEL1		=(0xF<<25);


//===============
// Command-Flags
//===============

constexpr uint32_t CMF_TYPE_NORMAL		=(0<<22);
constexpr uint32_t CMF_TYPE_SUSPEND		=(1<<22);
constexpr uint32_t CMF_TYPE_RESUME		=(2<<22);
constexpr uint32_t CMF_TYPE_ABORT		=(3<<22);
constexpr uint32_t CMF_ISDATA			=(1<<21);
constexpr uint32_t CMF_IXCHK			=(1<<20);
constexpr uint32_t CMF_CRCCHK			=(1<<19);
constexpr uint32_t CMF_RSPNS_MASK		=(3<<16);
constexpr uint32_t CMF_RSPNS_136		=(1<<16);
constexpr uint32_t CMF_RSPNS_48			=(2<<16);
constexpr uint32_t CMF_RSPNS_48_BUSY	=(3<<16);
constexpr uint32_t CMF_MULTI_BLOCK		=(1<<5);
constexpr uint32_t CMF_DAT_DIR_READ		=(1<<4);
constexpr uint32_t CMF_DAT_DIR_WRITE	=(0<<4);
constexpr uint32_t CMF_AUTO_CMD_23		=(1<<3);
constexpr uint32_t CMF_AUTO_CMD_12		=(1<<2);
constexpr uint32_t CMF_BLKCNT_EN		=(1<<1);


//==========
// Commands
//==========

constexpr uint32_t CMD_GO_IDLE			=(0<<24);
constexpr uint32_t CMD_SEND_CID			=(2<<24)|CMF_RSPNS_136|CMF_CRCCHK;
constexpr uint32_t CMD_SEND_REL_ADDR	=(3<<24)|CMF_RSPNS_48|CMF_CRCCHK;
constexpr uint32_t CMD_SEND_OP_COND		=(5<<24)|CMF_RSPNS_48;
constexpr uint32_t CMD_SELECT_CARD		=(7<<24)|CMF_RSPNS_48_BUSY|CMF_CRCCHK;
constexpr uint32_t CMD_SEND_IF_COND		=(8<<24)|CMF_RSPNS_48|CMF_CRCCHK;
constexpr uint32_t CMD_SEND_CSD			=(9<<24)|CMF_RSPNS_136|CMF_CRCCHK;
constexpr uint32_t CMD_READ_SINGLE		=(17<<24)|CMF_RSPNS_48|CMF_CRCCHK|CMF_ISDATA|CMF_DAT_DIR_READ;
constexpr uint32_t CMD_READ_MULTI		=(18<<24)|CMF_RSPNS_48|CMF_CRCCHK|CMF_ISDATA|CMF_DAT_DIR_READ|CMF_MULTI_BLOCK|CMF_BLKCNT_EN|CMF_AUTO_CMD_12;
constexpr uint32_t CMD_WRITE_SINGLE		=(24<<24)|CMF_RSPNS_48|CMF_CRCCHK|CMF_ISDATA|CMF_DAT_DIR_WRITE;
constexpr uint32_t CMD_WRITE_MULTI		=(25<<24)|CMF_RSPNS_48|CMF_CRCCHK|CMF_ISDATA|CMF_DAT_DIR_WRITE|CMF_MULTI_BLOCK|CMF_BLKCNT_EN|CMF_AUTO_CMD_12;
constexpr uint32_t CMD_IO_RW_DIRECT		=(52<<24)|CMF_RSPNS_48|CMF_CRCCHK|CMF_IXCHK;
constexpr uint32_t CMD_IO_RW_EXTENDED	=(53<<24)|CMF_RSPNS_48|CMF_CRCCHK|CMF_IXCHK|CMF_ISDATA;
constexpr uint32_t CMD_APPCMD			=(55<<24)|CMF_RSPNS_48|CMF_CRCCHK;

// CMD_IO_RW
constexpr uint32_t IO_RW_WRITE	=(1<<31);
constexpr uint32_t IO_RW_READ	=(0<<31);
constexpr bits32_t IO_RW_FUNC	={ 0x7, 28 };
constexpr uint32_t IO_RW_BLK	=(1<<27);
constexpr uint32_t IO_RW_INCR	=(1<<26);
constexpr bits32_t IO_RW_ADDR	={ 0x1FFFF, 9};
constexpr bits32_t IO_RW_COUNT	={ 0x1FF, 0};
constexpr bits32_t IO_RW_DATA	={ 0xFF, 0};

// CMD_SEND_IF_COND
constexpr uint32_t IF_COND_DEFAULT	=0x1AA;

// CMD_SEND_OP_COND
constexpr uint32_t OP_COND_3V3			=(3<<20);
constexpr uint32_t OP_COND_SUCCESS		=(1<<31);

// CMD_SEND_REL_ADDR
constexpr bits32_t SEND_REL_ADDR_RCA	={ 0xFFFF, 16 };


//==============
// App-Commands
//==============

constexpr uint32_t ACMD_SEND_OP_COND	=(41<<24)|CMF_RSPNS_48;


//===========
// Functions
//===========

constexpr uint32_t FN0=0;
constexpr uint32_t FN1=1;
constexpr uint32_t FN2=2;

}}