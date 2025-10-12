//========
// Pcie.h
//========

#pragma once


//=======
// Using
//=======

#include <base.h>
#include <io.h>


//===========
// Namespace
//===========

namespace Devices {
	namespace Pcie {


//==========
// Settings
//==========

constexpr SIZE_T PCIE_OFFSET	=0x0000000000;

constexpr SIZE_T RBAR1_OFFSET	=0xFFFFFFF000; // MSI - 0xFFFFFFF000
constexpr SIZE_T RBAR1_SIZE		=0x1000;

constexpr SIZE_T RBAR2_OFFSET	=AXI_IO_BASE; // AXI - 0x1000000000
constexpr SIZE_T RBAR2_SIZE		=0x100000000;

constexpr SIZE_T OUTWND_OFFSET	=RP1_IO_BASE; // RP1 - 0x1F00000000
constexpr SIZE_T OUTWND_SIZE	=0xFFFFFFFC;

constexpr SIZE_T DMA_OFFSET		=0x1000000000; // 0x1000000000 (64GB)
constexpr SIZE_T DMA_SIZE		=0x1000000000; // 0x1000000000 (64GB)


//=====
// Bus
//=====

enum
{
PCIE_BUS_ONBOARD=0,
PCIE_BUS_EXTERNAL=1
};


//========
// Device
//========

typedef struct
{
uint32_t RES0;
rw16_t CMD;
uint16_t RES1;
rw32_t CLASSREV;
rw8_t CACHE_LINE_SIZE;
rw8_t LATENCY_TIMER;
rw8_t HEADER_TYPE;
uint8_t RES2;
rw32_t BASE_LO;
rw32_t BASE_HI;
rw8_t PRIMARY_BUS;
rw8_t SECONDARY_BUS;
rw8_t SUBORDINATE_BUS;
rw8_t SEC_LATENCY_TIMER;
rw32_t RES3;
rw16_t MEM_BASE;
rw16_t MEM_LIMIT;
rw32_t RES4[6];
rw8_t IRQ_LINE;
rw8_t IRQ_PIN;
rw8_t BRIDGE_CTRL;
uint8_t RES5;
rw32_t RES6[27];
rw32_t CAPS;
rw32_t DEVCAP;
rw32_t DEVCTRL;
rw32_t LNKCAP;
rw32_t LNKCTRL;
rw32_t SLOT_CAP;
rw32_t SLOT_CTRL;
rw8_t ROOT_CTRL;
uint8_t RES7[3];
rw32_t ROOT_STATUS;
rw32_t DEVCAP2;
rw32_t DEVCTRL2;
rw32_t LNKCAP2;
rw16_t LNKCTL2;
uint16_t RES8;
rw32_t SLOT_CAP2;
rw32_t SLOT_CTRL2;
rw32_t RES9[40];
rw32_t VENDOR_SPECIFIC_REG1;
rw32_t RES10[172];
rw32_t PRIV1_ID_VAL3;
rw32_t RES11[39];
rw32_t PRIV1_LINK_CAPABILITY;
rw32_t RES12[331];
rw32_t TL_VDM_CTL1;
rw32_t RES13[4];
rw32_t TL_VDM_CTL0;
rw32_t RES14[375];
}pcie_regs_t;

constexpr uint32_t BASE_MEM_TYPE_64=4;

constexpr uint32_t BRIDGE_CTRL_PARITY=1;

constexpr bits32_t CLASSREV_CLASS={ 0xFFFFFF, 8 };
constexpr uint32_t CLASSREV_CLASS_HOST=0x060400;

constexpr uint16_t CMD_MEMORY=(1<<1);
constexpr uint16_t CMD_MASTER=(1<<2);
constexpr uint16_t CMD_PARITY=(1<<6);
constexpr uint16_t CMD_SERR=(1<<8);

enum
{
HEADER_TYPE_NORMAL=0,
HEADER_TYPE_BRIDGE=1
};

constexpr uint32_t LINK_STATE_L0S=(1<<0);
constexpr uint32_t LINK_STATE_L1=(1<<1);

constexpr bits32_t PRIV1_LINK_CAPABILITY_ASPM_SUPPORT={ 0x3, 10 };

constexpr uint8_t ROOT_CTRL_CRSSVE=(1<<4);

constexpr uint32_t TL_VDM_CTL0_VDM_IGNOREVNDRID=(1<<18);
constexpr uint32_t TL_VDM_CTL0_VDM_IGNORETAG=(1<<17);
constexpr uint32_t TL_VDM_CTL0_VDM_ENABLED=(1<<16);


//======
// Host
//======

typedef struct
{
rw32_t LOW;
rw32_t HIGH;
}mem_win_t;

typedef struct
{
rw32_t BASE;
rw32_t LIMIT;
}mem_win_base_limit_t;

typedef struct
{
pcie_regs_t CFG;
uint32_t RES9[64];
rw32_t DL_MDIO_ADDR;
rw32_t DL_MDIO_WR_DATA;
rw32_t DL_MDIO_RD_DATA;
uint32_t RES10[464];
rw32_t PL_PHY_CTL_15;
uint32_t RES11[2542];
rw32_t MISC_CTRL;
mem_win_t CPU2_MEM_WIN[4];
rw32_t RC_BAR1_CONFIG_LO;
rw32_t RC_BAR1_CONFIG_HI;
rw32_t RC_BAR2_CONFIG_LO;
rw32_t RC_BAR2_CONFIG_HI;
rw32_t RC_BAR3_CONFIG_LO;
rw32_t RC_BAR3_CONFIG_HI;
rw32_t MSI_BAR_CONFIG_LO;
rw32_t MSI_BAR_CONFIG_HI;
rw32_t MSI_DATA_CONFIG;
uint32_t RES12[3];
rw32_t RC_CONFIG_RETRY_TIMEOUT;
rw32_t EOI_CTRL;
rw32_t PCIE_CTRL;
rw32_t PCIE_STATUS;
rw32_t REVISION;
rw32_t CPU2_MEM_WIN_BASE_LIMIT[4];
mem_win_base_limit_t CPU2_MEM_WIN_BASE_LIMIT_HI[4];
rw32_t MISC_CTRL1;
rw32_t UBUS_CTRL;
rw32_t UBUS_TIMEOUT;
rw32_t UBUS_BAR1_CONFIG_REMAP;
rw32_t UBUS_BAR1_CONFIG_REMAP_HI;
rw32_t UBUS_BAR2_CONFIG_REMAP;
uint32_t RES13[43];
rw32_t VDM_PRIORITY_TO_QOS_MAP_HI;
rw32_t VDM_PRIORITY_TO_QOS_MAP_LO;
rw32_t AXI_INTF_CTRL;
rw32_t AXI_READ_ERROR_DATA;
uint32_t RES14[100];
rw32_t HARD_DEBUG;
uint32_t RES15[62];
rw32_t INTR2_CPU_BASE[64];
rw32_t INTR2_MSI_BASE[64];
uint32_t RES16[3712];
pcie_regs_t CFG_DATA;
rw32_t CFG_INDEX;
}pcie_host_regs_t;

constexpr uint32_t AXI_INTF_CTRL_REQFIFO_EN_QOS_PROPAGATION=(1<<7);

constexpr bits32_t CFG_INDEX_BUS={ 0xFFF, 20 };
constexpr bits32_t CFG_INDEX_FUNC={ 0x7, 12 };
constexpr bits32_t CFG_INDEX_SLOT={ 0x1F, 15 };

constexpr bits32_t CPU2_MEM_WIN_BASE_LIMIT_LIMIT={ 0xFFF, 20 };
constexpr bits32_t CPU2_MEM_WIN_BASE_LIMIT_BASE={ 0xFFF, 4 };
constexpr uint32_t CPU2_MEM_WIN_BASE_LIMIT_NUM_BITS=12;
constexpr uint32_t CPU2_MEM_WIN_BASE_LIMIT_HI_MASK=0xFF;

constexpr uint32_t HARD_DEBUG_CLKREQ_L1SS_EN=(1<<21);

constexpr bits32_t MISC_CTRL_SCB0_SIZE={ 0x1F, 27 };
constexpr bits32_t MISC_CTRL_MAX_BURST_SIZE={ 0x3, 20 };
constexpr uint32_t MISC_CTRL_MAX_BURST_SIZE_256=1;
constexpr uint32_t MISC_CTRL_CFG_READ_UR_MODE=(1<<13);
constexpr uint32_t MISC_CTRL_SCB_ACCESS_EN=(1<<12);
constexpr uint32_t MISC_CTRL_RCB_MPS_MODE=(1<<10);

constexpr uint32_t MISC_CTRL1_EN_VDM_QOS_CONTROL=(1<<5);
constexpr uint32_t MISC_CTRL1_OUTBOUND_RO=(1<<4);
constexpr uint32_t MISC_CTRL1_OUTBOUND_NO_SNOOP=(1<<3);
constexpr uint32_t MISC_CTRL1_OUTBOUND_TC_MASK=0xF;

constexpr uint32_t PCIE_CTRL_PERST=(1<<2);

constexpr uint32_t PCIE_STATUS_DL_ACTIVE=(1<<5);
constexpr uint32_t PCIE_STATUS_PHYLINKUP=(1<<4);

constexpr bits32_t PL_PHY_CTL_15_CLOCK_PERIOD={ 0xFF, 0 };
constexpr uint32_t PL_PHY_CTL_15_CLOCK_PERIOD_DEFAULT=0x12;

constexpr uint32_t RC_BAR_CONFIG_LO_SIZE=0x1F;

constexpr uint32_t UBUS_BAR_CONFIG_REMAP_ACCESS_EN=(1<<0);

constexpr uint32_t UBUS_CTRL_REPLY_DECERR_DIS=(1<<19);
constexpr uint32_t UBUS_CTRL_REPLY_ERR_DIS=(1<<13);


//======
// Mdio
//======

constexpr uint8_t MDIO_REG_SET_ADDR_OFFSET=0x1F;

constexpr bits32_t MDIO_PORT={ 0xF, 22 };
constexpr bits32_t MDIO_CMD={ 0x3, 20 };
constexpr bits32_t MDIO_REG={ 0xFFFF, 0 };
constexpr uint32_t MDIO_DATA_DONE=(1<<31);
constexpr uint32_t MDIO_DATA_MASK=0x7FFFFFFF;

enum
{
MDIO_CMD_WRITE=0,
MDIO_CMD_READ=1
};


//============
// Interrupts
//============

constexpr uint32_t RP1_IRQ_COUNT=48;
constexpr uint64_t RP1_IRQ_EDGE_MASK=0x18C0000000ULL;

typedef VOID (*IRQ_HANDLER)(VOID* Parameter);

enum class Rp1Irq
{
IoBank0=0,
IoBank1=1,
IoBank2=2,
Ethernet=6,
I2s0=14,
I2s1=15,
UArt0=25,
Xhci0=30,
Xhci0_0=31,
Xhci1=35,
Xhci1_0=36,
Dma=40,
UArt1=42,
UArt2=43,
UArt3=44,
UArt4=45,
UArt5=46
};

typedef struct
{
rw32_t REG_RW;
uint32_t RES0[64];
rw32_t INT_STATUS[2];
uint32_t RES1[447];
rw32_t MSIX_CFG_SET[64];
uint32_t RES2[192];
rw32_t MSIX_CFG_CLEAR[64];
}rp1_intc_regs_t;

constexpr uint32_t MSIX_CFG_IACK_EN=(1<<3);
constexpr uint32_t MSIX_CFG_IACK=(1<<2);
constexpr uint32_t MSIX_CFG_EN=(1<<0);

}}