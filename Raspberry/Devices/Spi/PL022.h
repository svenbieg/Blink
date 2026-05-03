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
const BITS CTRL0_FRF	={ 0x3, 4 };
const BITS CTRL0_DSS	={ 0xF, 0 };

typedef enum
{
FRF_TI=1
}FRF;

typedef enum
{
DSS_4BIT=3,
DSS_8BIT=7,
DSS_16BIT=15
}DSS;

const UINT CTRL1_EN		=(1<<1);

const UINT STATUS_BUSY	=(1<<4);
const UINT STATUS_RF	=(1<<3);
const UINT STATUS_RNE	=(1<<2);
const UINT STATUS_TNF	=(1<<1);
const UINT STATUS_TE	=(1<<0);

const UINT DMACTRL_TXEN	=(1<<1);
const UINT DMACTRL_RXEN	=(1<<0);

}}