//=======
// Pio.h
//=======

#pragma once


//=======
// Using
//=======

#include "Devices/IoHelper.h"
#include <base.h>


//===========
// Namespace
//===========

namespace Devices {
	namespace Pio {


//==========
// Settings
//==========

const UINT PIO_COUNT				=3;
const UINT PIO_INSTR_COUNT			=32;
const UINT PIO_SM_COUNT				=4;

const UINT PIO_BASE[PIO_COUNT]={ PIO0_BASE, PIO1_BASE, PIO2_BASE };


//===========
// Registers
//===========

typedef struct
{
RW32 CLK_DIV;
RW32 EXEC_CTRL;
RW32 SHIFT_CTRL;
RO32 ADDR;
RW32 INSTR;
RW32 PIN_CTRL;
}PIO_SM_REGS;

const BITS CLK_DIV_INT					={ 0x0F, 16 };
const BITS CLK_DIV_FRAC					={ 0x0F, 8 };

const UINT EXEC_CTRL_STALLED			=(1<<31);
const BITS EXEC_CTRL_SIDESET_OPT		={ 0x01, 30 };
const BITS EXEC_CTRL_SIDESET_PINDIR		={ 0x01, 29 };
const UINT EXEC_CTRL_STICKY				=(1<<17);
const BITS EXEC_CTRL_WRAP				={ 0x1F, 12 };
const BITS EXEC_CTRL_WRAP_TARGET		={ 0x1F, 7 };

enum OPCODE
{
OPCODE_JMP=0x0,
OPCODE_OUT=0x6,
OPCODE_SET=0xE
};

const BITS INSTR_OPCODE					={ 0x7, 13 };
const BITS INSTR_ARG1					={ 0x7, 5 };
const BITS INSTR_ARG2					={ 0x1F, 0 };
const UINT INSTR_OUT					=(OPCODE_OUT<<12);
const UINT INSTR_SET_PINS				=(OPCODE_SET<<12)|(0<<5);
const UINT INSTR_SET_PINDIR				=(OPCODE_SET<<12)|(4<<5);

const BITS PIN_CTRL_SIDESET_COUNT		={ 0x07, 29 };
const BITS PIN_CTRL_SET_COUNT			={ 0x07, 26 };
const BITS PIN_CTRL_OUT_COUNT			={ 0x3F, 20 };
const BITS PIN_CTRL_IN_BASE				={ 0x1F, 15 };
const BITS PIN_CTRL_SIDESET_BASE		={ 0x1F, 10 };
const BITS PIN_CTRL_SET_BASE			={ 0x1F, 5 };
const BITS PIN_CTRL_OUT_BASE			={ 0x1F, 0 };

const UINT SHIFT_CTRL_FJOIN_RX			=(1<<31);
const BITS SHIFT_CTRL_PULL_TRESH		={ 0x1F, 25 };
const BITS SHIFT_CTRL_PUSH_TRESH		={ 0x1F, 20 };
const BITS SHIFT_CTRL_OUT_SHIFT_RIGHT	={ 0x01, 19 };
const BITS SHIFT_CTRL_IN_SHIFT_RIGHT	={ 0x01, 18 };
const BITS SHIFT_CTRL_AUTO_PULL			={ 0x01, 17 };
const BITS SHIFT_CTRL_AUTO_PUSH			={ 0x01, 16 };
const BITS SHIFT_CTRL_IN_COUNT			={ 0x1F, 0 };

typedef struct
{
RW32 INTE;
RW32 INTF;
RO32 INTS;
}PIO_INT_REGS;

typedef struct
{
RW32 CTRL;
RW32 FSTAT;
RW32 FDEBUG;
RW32 FLEVEL;
RW32 TX_FIFO[PIO_SM_COUNT];
RW32 RX_FIFO[PIO_SM_COUNT];
RW32 IRQ;
RW32 IRQ_FORCE;
RW32 INPUT_SYNC_BYPASS;
RO32 DBG_PADOUT;
RO32 DBG_PADOE;
RO32 DBG_CFGINFO;
RW32 INSTR_MEM[PIO_INSTR_COUNT];
PIO_SM_REGS SM[PIO_SM_COUNT];
RW32 RX_FIFO_PUT_GET[4][4];
RW32 GPIOBASE;
RW32 INTR;
PIO_INT_REGS IRQ_CTRL[2];
}PIO_REGS;

const UINT CTRL_SM_ENABLE=(1<<0);
const UINT CTRL_SM_RESTART=(1<<4);
const UINT CTRL_SM_CLKDIV_RESTART=(1<<8);

}}