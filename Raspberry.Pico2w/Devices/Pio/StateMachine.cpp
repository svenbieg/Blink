//==================
// StateMachine.cpp
//==================

#include "StateMachine.h"


//=======
// Using
//=======

#include "Concurrency/WriteLock.h"
#include "Devices/Pio/Pio.h"
#include "BitHelper.h"

using namespace Concurrency;
using namespace Devices::Dma;
using namespace Devices::Gpio;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Pio {


//=========
// Globals
//=========

UINT g_InstructionsUsed[PIO_COUNT]={ 0 };
Mutex g_Mutex;
BYTE g_StateMachinesUsed[PIO_COUNT]={ 0 };


//==================
// Con-/Destructors
//==================

StateMachine::~StateMachine()
{
Stop();
WriteLock lock(g_Mutex);
BitHelper::Clear(g_InstructionsUsed[m_Pio], m_InstructionsUsed);
BitHelper::Clear(g_StateMachinesUsed[m_Pio], 1U<<m_StateMachine);
}

Handle<StateMachine> StateMachine::Create(PIO_PROGRAM const& program)
{
UINT program_mask=(1U<<program.InstructionCount)-1;
WriteLock lock(g_Mutex);
for(INT pio_id=PIO_COUNT-1; pio_id>=0; pio_id--)
	{
	if(g_StateMachinesUsed[pio_id]==0xF)
		continue;
	for(INT offset=PIO_INSTR_COUNT-program.InstructionCount; offset>=0; offset--)
		{
		UINT used_mask=program_mask<<offset;
		if(BitHelper::Get(g_InstructionsUsed[pio_id], used_mask))
			continue;
		Reset(pio_id);
		UploadProgram(pio_id, offset, program);
		g_InstructionsUsed[pio_id]|=used_mask;
		UINT sm=GetStateMachine(pio_id);
		return new StateMachine(pio_id, sm, used_mask);
		}
	}
throw DeviceNotReadyException();
}


//========
// Common
//========

VOID StateMachine::ClearBuffers()
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
IoHelper::Flip(sm->SHIFT_CTRL, SHIFT_CTRL_FJOIN_RX);
IoHelper::Flip(sm->SHIFT_CTRL, SHIFT_CTRL_FJOIN_RX);
}

RO32* StateMachine::GetInputBuffer()
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
return &pio->RX_FIFO[m_StateMachine];
}

DmaRequest StateMachine::GetInputRequest()
{
return (DmaRequest)(m_Pio*(PIO_SM_COUNT*2)+(PIO_SM_COUNT+m_StateMachine));
}

RW32* StateMachine::GetOutputBuffer()
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
return &pio->TX_FIFO[m_StateMachine];
}

DmaRequest StateMachine::GetOutputRequest()
{
return (DmaRequest)(m_Pio*(PIO_SM_COUNT*2)+m_StateMachine);
}

GpioPinMode StateMachine::GetPinMode()
{
return (GpioPinMode)(6+m_Pio);
}

VOID StateMachine::Jump(WORD addr)
{
UINT offset=Cpu::CountTrailingZeros(m_InstructionsUsed);
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
IoHelper::Write(sm->INSTR, offset+addr);
}

VOID StateMachine::Output(PioOutput dst, UINT value, UINT bit_count)
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
pio->TX_FIFO[m_StateMachine]=value;
UINT instr=INSTR_OUT;
BitHelper::Set(instr, INSTR_ARG1, (UINT)dst);
BitHelper::Set(instr, INSTR_ARG2, bit_count);
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
IoHelper::Write(sm->INSTR, instr);
}

VOID StateMachine::Restart()
{
UINT ctrl=0;
BitHelper::Set(ctrl, CTRL_SM_RESTART<<m_StateMachine);
BitHelper::Set(ctrl, CTRL_SM_CLKDIV_RESTART<<m_StateMachine);
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
IoHelper::Write(pio->CTRL, ctrl);
}

VOID StateMachine::SetDivisor(BYTE div, BYTE frac)
{
UINT clk_div=0;
BitHelper::Set(clk_div, CLK_DIV_INT, div);
BitHelper::Set(clk_div, CLK_DIV_FRAC, frac);
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
IoHelper::Write(sm->CLK_DIV, clk_div);
}

VOID StateMachine::SetInputPin(GpioPin pin)
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
IoHelper::Set(sm->PIN_CTRL, PIN_CTRL_IN_BASE, (UINT)pin);
}

VOID StateMachine::SetInputShift(BOOL shift_right, BOOL auto_push, UINT push_tresh)
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
UINT shift_ctrl=IoHelper::Read(sm->SHIFT_CTRL);
BitHelper::Set(shift_ctrl, SHIFT_CTRL_IN_SHIFT_RIGHT, shift_right);
BitHelper::Set(shift_ctrl, SHIFT_CTRL_AUTO_PUSH, auto_push);
BitHelper::Set(shift_ctrl, SHIFT_CTRL_PUSH_TRESH, push_tresh);
IoHelper::Write(sm->SHIFT_CTRL, shift_ctrl);
}

VOID StateMachine::SetInputSyncBypass(GpioPin pin)
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
IoHelper::Set(pio->INPUT_SYNC_BYPASS, 1U<<(UINT)pin);
}

VOID StateMachine::SetOutputPins(GpioPin pin, BYTE count)
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
UINT pin_ctrl=IoHelper::Read(sm->PIN_CTRL);
BitHelper::Set(pin_ctrl, PIN_CTRL_OUT_COUNT, count);
BitHelper::Set(pin_ctrl, PIN_CTRL_OUT_BASE, (UINT)pin);
IoHelper::Write(sm->PIN_CTRL, pin_ctrl);
}

VOID StateMachine::SetOutputShift(BOOL shift_right, BOOL auto_pull, UINT pull_tresh)
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
UINT shift_ctrl=IoHelper::Read(sm->SHIFT_CTRL);
BitHelper::Set(shift_ctrl, SHIFT_CTRL_OUT_SHIFT_RIGHT, shift_right);
BitHelper::Set(shift_ctrl, SHIFT_CTRL_AUTO_PULL, auto_pull);
BitHelper::Set(shift_ctrl, SHIFT_CTRL_PULL_TRESH, pull_tresh);
IoHelper::Write(sm->SHIFT_CTRL, shift_ctrl);
}

VOID StateMachine::SetPinDirection(GpioPin pin, PioPinDirection dir)
{
UINT cmd=INSTR_SET_PINDIR|(UINT)dir;
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
UINT exec_ctrl=IoHelper::Read(sm->EXEC_CTRL);
UINT pin_ctrl=IoHelper::Read(sm->PIN_CTRL);
IoHelper::Clear(sm->EXEC_CTRL, EXEC_CTRL_STICKY);
UINT ctrl=0;
BitHelper::Set(ctrl, PIN_CTRL_SET_BASE, (UINT)pin);
BitHelper::Set(ctrl, PIN_CTRL_SET_COUNT, 1);
IoHelper::Write(sm->PIN_CTRL, ctrl);
IoHelper::Write(sm->INSTR, cmd);
IoHelper::Write(sm->PIN_CTRL, pin_ctrl);
IoHelper::Write(sm->EXEC_CTRL, exec_ctrl);
}

VOID StateMachine::SetPins(BOOL value)
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
IoHelper::Write(sm->INSTR, INSTR_SET_PINS|value);
}

VOID StateMachine::SetSetPins(GpioPin pin, BYTE count)
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
UINT pin_ctrl=IoHelper::Read(sm->PIN_CTRL);
BitHelper::Set(pin_ctrl, PIN_CTRL_SET_COUNT, count);
BitHelper::Set(pin_ctrl, PIN_CTRL_SET_BASE, (UINT)pin);
IoHelper::Write(sm->PIN_CTRL, pin_ctrl);
}

VOID StateMachine::SideSet(GpioPin pin, BYTE bit_count, BOOL opt, BOOL dir)
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
UINT exec_ctrl=IoHelper::Read(sm->EXEC_CTRL);
UINT pin_ctrl=IoHelper::Read(sm->PIN_CTRL);
BitHelper::Set(pin_ctrl, PIN_CTRL_SIDESET_BASE, (UINT)pin);
BitHelper::Set(pin_ctrl, PIN_CTRL_SIDESET_COUNT, bit_count);
BitHelper::Set(exec_ctrl, EXEC_CTRL_SIDESET_OPT, opt);
BitHelper::Set(exec_ctrl, EXEC_CTRL_SIDESET_PINDIR, dir);
IoHelper::Write(sm->EXEC_CTRL, exec_ctrl);
IoHelper::Write(sm->PIN_CTRL, pin_ctrl);
}

VOID StateMachine::Start()
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
IoHelper::Set(pio->CTRL, CTRL_SM_ENABLE<<m_StateMachine);
}

VOID StateMachine::Stop()
{
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
IoHelper::Clear(pio->CTRL, CTRL_SM_ENABLE<<m_StateMachine);
}

VOID StateMachine::Wrap(WORD wrap_target, WORD wrap)
{
UINT offset=Cpu::CountTrailingZeros(m_InstructionsUsed);
wrap_target+=offset;
wrap+=offset;
auto pio=(PIO_REGS*)PIO_BASE[m_Pio];
auto sm=(PIO_SM_REGS*)&pio->SM[m_StateMachine];
UINT exec_ctrl=IoHelper::Read(sm->EXEC_CTRL);
BitHelper::Set(exec_ctrl, EXEC_CTRL_WRAP_TARGET, wrap_target);
BitHelper::Set(exec_ctrl, EXEC_CTRL_WRAP, wrap);
IoHelper::Write(sm->EXEC_CTRL, exec_ctrl);
}


//==========================
// Con-/Destructors Private
//==========================

StateMachine::StateMachine(BYTE pio_id, BYTE sm, UINT instr_used):
m_InstructionsUsed(instr_used),
m_Pio(pio_id),
m_StateMachine(sm)
{}


//================
// Common Private
//================

UINT StateMachine::GetStateMachine(UINT pio)noexcept
{
BYTE mask=1;
for(UINT sm=0; sm<4; sm++)
	{
	if((g_StateMachinesUsed[pio]&mask))
		{
		mask<<=1;
		continue;
		}
	g_StateMachinesUsed[pio]|=mask;
	return sm;
	}
return 0;
}

VOID StateMachine::Reset(UINT pio)
{
using System=Devices::System::System;
UINT id=(UINT)ResetDevice::Pio0<<pio;
System::Enable((ResetDevice)id);
}

VOID StateMachine::UploadProgram(UINT pio_id, UINT offset, PIO_PROGRAM const& program)
{
auto pio=(PIO_REGS*)PIO_BASE[pio_id];
UINT pos=offset;
for(UINT u=0; u<program.InstructionCount; u++)
	{
	WORD instr=program.Instructions[u];
	if(BitHelper::Get(instr, INSTR_OPCODE)==OPCODE_JMP)
		instr+=offset;
	pio->INSTR_MEM[pos++]=instr;
	}
}

}}