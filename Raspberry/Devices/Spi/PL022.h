//=========
// PL022.h
//=========

#pragma once


//=======
// Using
//=======

#include "Devices/IoHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Spi {


//==========
// Settings
//==========

const UINT PL022_RX_BYTES=2*8;
const UINT PL022_TX_BYTES=2*8;


//===========
// Registers
//===========

typedef struct
{
RW32 CTRL0;
RW32 CTRL1;
RW32 DATA;
RO32 STATUS;
RW32 PRESCAL;
RW32 IRQMSC;
RO32 IRQS;
RO32 IRQMS;
RW32 IRQC;
RW32 DMACTRL;
}PL022_REGS;

const BITS CTRL0_SCR	={ 0xFF, 8 };
const UINT CTRL0_SPH	=(1<<7);
const UINT CTRL0_SPO	=(1<<6);
const BITS CTRL0_FRF	={ 0x3, 4 };
const BITS CTRL0_DSS	={ 0xF, 0 };

typedef enum
{
FRF_MOTOROLA=0,
FRF_TI=1,
FRF_MICROWIRE=2
}FRF;

typedef enum
{
DSS_4BIT=3,
DSS_8BIT=7,
DSS_16BIT=15
}DSS;

const UINT CTRL1_SOD	=(1<<3);
const UINT CTRL1_MS		=(1<<2);
const UINT CTRL1_SSE	=(1<<1);
const UINT CTRL1_LBM	=(1<<0);

const UINT DMACTRL_TXEN	=(1<<1);
const UINT DMACTRL_RXEN	=(1<<0);

const UINT IRQS_TXNF	=(1<<3);
const UINT IRQS_RXNE	=(1<<2);
const UINT IRQS_TIMEOUT	=(1<<1);
const UINT IRQS_OVERRUN	=(1<<0);
const UINT IRQS_MASK	=0xF;

const BITS PRESCAL_DIV	={ 0xFF, 0 };

const UINT STATUS_BUSY	=(1<<4);
const UINT STATUS_RFF	=(1<<3);
const UINT STATUS_RNE	=(1<<2);
const UINT STATUS_TNF	=(1<<1);
const UINT STATUS_TFE	=(1<<0);

}}