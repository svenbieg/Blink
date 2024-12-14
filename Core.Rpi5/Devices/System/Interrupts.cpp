//================
// Interrupts.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Exceptions.h"
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

extern "C" VOID HandleInterrupt()noexcept
{
auto gicc=(GICC_REGS*)ARM_GICC_BASE;
UINT iar=Bits::Get(gicc->IAR);
UINT irq=Bits::Get(iar, GICC_IAR_IRQ_MASK);
if(irq>=IRQ_COUNT)
	return;
Interrupts::HandleInterrupt(irq);
Bits::Write(gicc->EOIR, iar);
}


//========
// Common
//========

VOID Interrupts::Disable()noexcept
{
Cpu::DisableInterrupts();
UINT core=Cpu::GetId();
s_DisableCount[core]++;
}

VOID Interrupts::Disable(UINT irq)noexcept
{
assert(irq<IRQ_COUNT);
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT reg=irq/32;
UINT mask=1<<(irq%32);
Bits::Set(gicd->CLEAR_ENABLED[reg], mask);
}

VOID Interrupts::Enable()noexcept
{
UINT core=Cpu::GetId();
if(--s_DisableCount[core]==0)
	Cpu::EnableInterrupts();
}

VOID Interrupts::Enable(UINT irq)noexcept
{
assert(irq<IRQ_COUNT);
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT reg=irq/32;
UINT mask=1<<(irq%32);
Bits::Set(gicd->SET_ENABLED[reg], mask);
}

VOID Interrupts::HandleInterrupt(UINT irq)noexcept
{
UINT core=Cpu::GetId();
s_DisableCount[core]++;
if(s_IrqHandler[irq])
	s_IrqHandler[irq](s_IrqParameter[irq]);
s_DisableCount[core]--;
}

VOID Interrupts::Initialize()noexcept
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
Bits::Write(gicd->CTRL, GICD_CTRL_DISABLE);
for(UINT u=0; u<IRQ_COUNT/32; u++)
	{
	Bits::Write(gicd->CLEAR_ENABLED[u], 0xFFFFFFFF);
	Bits::Write(gicd->CLEAR_PENDING[u], 0xFFFFFFFF);
	Bits::Write(gicd->CLEAR_ACTIVE[u], 0xFFFFFFFF);
	}
for(UINT u=0; u<IRQ_COUNT/4; u++)
	{
	Bits::Write(gicd->PRIORITY[u], GICD_PRIORITY_DEFAULT);
	Bits::Write(gicd->TARGET[u], GICD_TARGET_CORE0);
	}
for(UINT u=0; u<IRQ_COUNT/16; u++)
	{
	Bits::Write(gicd->CONFIG[u], GICD_CONFIG_LEVEL_TRIGGERED);
	}
Bits::Write(gicd->CTRL, GICD_CTRL_ENABLE_GROUP0);
auto gicc=(GICC_REGS*)ARM_GICC_BASE;
Bits::Write(gicc->PMR, GICC_PMR_PRIORITY);
Bits::Write(gicc->CTRL, GICC_CTRL_ENABLE);
for(UINT core=0; core<CPU_COUNT; core++)
	s_DisableCount[core]=1;
}

VOID Interrupts::Route(UINT irq, IrqTarget target)noexcept
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT reg=irq/4;
UINT id=irq%4;
UINT mask=0xF<<id;
UINT value=(UINT)target<<id;
Bits::Set(gicd->TARGET[reg], mask, value);
}

VOID Interrupts::Send(UINT irq, IrqTarget target)noexcept
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT value=0;
Bits::Set(value, GICD_SGIR_CPU_TARGET_LIST, (UINT)target);
Bits::Set(value, irq);
Bits::Write(gicd->SGIR, value);
}

VOID Interrupts::SetHandler(UINT irq, IRQ_HANDLER handler, VOID* param)noexcept
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

UINT Interrupts::s_DisableCount[CPU_COUNT]={ 0 };
IRQ_HANDLER Interrupts::s_IrqHandler[IRQ_COUNT]={ nullptr };
VOID* Interrupts::s_IrqParameter[IRQ_COUNT]={ nullptr };

}}
