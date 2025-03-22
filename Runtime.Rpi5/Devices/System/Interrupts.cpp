//================
// Interrupts.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include <base.h>
#include "Devices/System/Cpu.h"
#include "BitHelper.h"
#include "Interrupts.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//=================
// GIC Distributor
//=================

typedef struct
{
REG CTRL;
UINT RES0[31];
REG GROUP[32];
REG SET_ENABLED[32]; // 0x80
REG CLEAR_ENABLED[32]; // 0x180
REG SET_PENDING[32]; // 0x280
REG CLEAR_PENDING[32]; // 0x380
REG SET_ACTIVE[32]; // 0x480
REG CLEAR_ACTIVE[32]; // 0x580
REG PRIORITY[256]; // 0x800
REG TARGET[256]; // 0xA00
REG CONFIG[192]; // 0xC00
REG SGIR; // 0xF00
}GICD_REGS;

constexpr UINT GICD_CTRL_DISABLE=0;
constexpr UINT GICD_CTRL_ENABLE_GROUP0=(1<<0);
constexpr UINT GICD_CTRL_ENABLE_GROUP1=(1<<1);
constexpr UINT GICD_PRIORITY_DEFAULT=0xA0A0A0A0;
constexpr UINT GICD_TARGET_CORE0=0x01010101;
constexpr UINT GICD_CONFIG_LEVEL_TRIGGERED=0;
constexpr BIT_FIELD GICD_SGIR_CPU_TARGET_LIST={ 0xF, 16 };


//===================
// GIC Cpu-Interface
//===================

typedef struct
{
REG CTRL;
REG PMR;
UINT RES0;
REG IAR;
REG EOIR;
}GICC_REGS;

constexpr UINT GICC_CTRL_ENABLE=1;
constexpr UINT GICC_PMR_PRIORITY=0xF0;
constexpr UINT GICC_IAR_IRQ_MASK=0x3FF;


//=========
// Handler
//=========

extern "C" VOID HandleInterrupt()
{
auto gicc=(GICC_REGS*)ARM_GICC_BASE;
UINT iar=BitHelper::Get(gicc->IAR);
UINT irq=BitHelper::Get(iar, GICC_IAR_IRQ_MASK);
if(irq>=IRQ_COUNT)
	return;
Interrupts::HandleInterrupt(irq);
BitHelper::Write(gicc->EOIR, iar);
}


//========
// Common
//========

BOOL Interrupts::Active()
{
UINT core=Cpu::GetId();
return s_Active[core];
}

VOID Interrupts::Disable()
{
Cpu::DisableInterrupts();
UINT core=Cpu::GetId();
s_DisableCount[core]++;
}

VOID Interrupts::Disable(UINT irq)
{
assert(irq<IRQ_COUNT);
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT reg=irq/32;
UINT mask=1<<(irq%32);
BitHelper::Set(gicd->CLEAR_ENABLED[reg], mask);
}

VOID Interrupts::Enable()
{
UINT core=Cpu::GetId();
if(--s_DisableCount[core]==0)
	Cpu::EnableInterrupts();
}

VOID Interrupts::Enable(UINT irq)
{
assert(irq<IRQ_COUNT);
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT reg=irq/32;
UINT mask=1<<(irq%32);
BitHelper::Set(gicd->SET_ENABLED[reg], mask);
}

BOOL Interrupts::Enabled()
{
UINT core=Cpu::GetId();
return s_DisableCount[core]==0;
}

VOID Interrupts::HandleInterrupt(UINT irq)
{
UINT core=Cpu::GetId();
s_Active[core]=true;
s_DisableCount[core]++;
if(s_IrqHandler[irq])
	s_IrqHandler[irq](s_IrqParameter[irq]);
s_Active[core]=false;
s_DisableCount[core]--;
}

VOID Interrupts::Initialize()
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
BitHelper::Write(gicd->CTRL, GICD_CTRL_DISABLE);
for(UINT u=0; u<IRQ_COUNT/32; u++)
	{
	BitHelper::Write(gicd->CLEAR_ENABLED[u], 0xFFFFFFFF);
	BitHelper::Write(gicd->CLEAR_PENDING[u], 0xFFFFFFFF);
	BitHelper::Write(gicd->CLEAR_ACTIVE[u], 0xFFFFFFFF);
	}
for(UINT u=0; u<IRQ_COUNT/4; u++)
	{
	BitHelper::Write(gicd->PRIORITY[u], GICD_PRIORITY_DEFAULT);
	BitHelper::Write(gicd->TARGET[u], GICD_TARGET_CORE0);
	}
for(UINT u=0; u<IRQ_COUNT/16; u++)
	{
	BitHelper::Write(gicd->CONFIG[u], GICD_CONFIG_LEVEL_TRIGGERED);
	}
BitHelper::Write(gicd->CTRL, GICD_CTRL_ENABLE_GROUP0);
auto gicc=(GICC_REGS*)ARM_GICC_BASE;
BitHelper::Write(gicc->PMR, GICC_PMR_PRIORITY);
BitHelper::Write(gicc->CTRL, GICC_CTRL_ENABLE);
for(UINT core=0; core<CPU_COUNT; core++)
	s_DisableCount[core]=1;
}

VOID Interrupts::Route(UINT irq, IrqTarget target)
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT reg=irq/4;
UINT id=irq%4;
UINT mask=0xF<<id;
UINT value=(UINT)target<<id;
BitHelper::Set(gicd->TARGET[reg], mask, value);
}

VOID Interrupts::Send(UINT irq, IrqTarget target)
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT value=0;
BitHelper::Set(value, GICD_SGIR_CPU_TARGET_LIST, (UINT)target);
BitHelper::Set(value, irq);
BitHelper::Write(gicd->SGIR, value);
}

VOID Interrupts::SetHandler(UINT irq, IRQ_HANDLER handler, VOID* param)
{
assert(irq<IRQ_COUNT);
Disable(irq);
s_IrqHandler[irq]=handler;
s_IrqParameter[irq]=param;
if(handler)
	Enable(irq);
}


//================
// Common Private
//================

BOOL Interrupts::s_Active[CPU_COUNT]={ false };
UINT Interrupts::s_DisableCount[CPU_COUNT]={ 0 };
IRQ_HANDLER Interrupts::s_IrqHandler[IRQ_COUNT]={ nullptr };
VOID* Interrupts::s_IrqParameter[IRQ_COUNT]={ nullptr };

}}
