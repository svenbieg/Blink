//================
// Interrupts.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include <base.h>
#include <io.h>
#include "Devices/System/Cpu.h"
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
rw32_t CTRL;
rw32_t RES0[31];
rw32_t GROUP[32];
rw32_t SET_ENABLED[32];
rw32_t CLEAR_ENABLED[32];
rw32_t SET_PENDING[32];
rw32_t CLEAR_PENDING[32];
rw32_t SET_ACTIVE[32];
rw32_t CLEAR_ACTIVE[32];
rw32_t PRIORITY[256];
rw32_t TARGET[256];
rw32_t CONFIG[192];
rw32_t SGIR;
}gicd_regs_t;

constexpr uint32_t GICD_CTRL_DISABLE			=0;
constexpr uint32_t GICD_CTRL_ENABLE_GROUP0		=(1<<0);
constexpr uint32_t GICD_CTRL_ENABLE_GROUP1		=(1<<1);
constexpr uint32_t GICD_PRIORITY_DEFAULT		=0xA0A0A0A0;
constexpr uint32_t GICD_TARGET_CORE0			=0x01010101;
constexpr uint32_t GICD_CONFIG_LEVEL_TRIGGERED	=0;
constexpr bits32_t GICD_SGIR_CPU_TARGET_LIST	={ 0xF, 16 };


//===================
// GIC CPU-Interface
//===================

typedef struct
{
rw32_t CTRL;
rw32_t PMR;
rw32_t RES0;
rw32_t IAR;
rw32_t EOIR;
}gicc_regs_t;

constexpr uint32_t GICC_CTRL_ENABLE=1;
constexpr uint32_t GICC_PMR_PRIORITY=0xF0;
constexpr uint32_t GICC_IAR_IRQ_MASK=0x3FF;


//=============
// IRQ-Handler
//=============

extern "C" VOID HandleInterrupt()
{
auto gicc=(gicc_regs_t*)ARM_GICC_BASE;
UINT iar=io_read(gicc->IAR);
UINT irq=io_read(iar, GICC_IAR_IRQ_MASK);
assert(irq<IRQ_COUNT);
Interrupts::HandleInterrupt(irq);
io_write(gicc->EOIR, iar);
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
auto gicd=(gicd_regs_t*)ARM_GICD_BASE;
UINT reg=irq/32;
UINT mask=1<<(irq%32);
io_write(gicd->CLEAR_ENABLED[reg], mask);
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
auto gicd=(gicd_regs_t*)ARM_GICD_BASE;
UINT reg=irq/32;
UINT mask=1<<(irq%32);
io_write(gicd->SET_ENABLED[reg], mask);
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
auto gicd=(gicd_regs_t*)ARM_GICD_BASE;
io_write(gicd->CTRL, GICD_CTRL_DISABLE);
for(UINT u=0; u<IRQ_COUNT/32; u++)
	{
	io_write(gicd->CLEAR_ENABLED[u], 0xFFFFFFFF);
	io_write(gicd->CLEAR_PENDING[u], 0xFFFFFFFF);
	io_write(gicd->CLEAR_ACTIVE[u], 0xFFFFFFFF);
	}
for(UINT u=0; u<IRQ_COUNT/4; u++)
	{
	io_write(gicd->PRIORITY[u], GICD_PRIORITY_DEFAULT);
	io_write(gicd->TARGET[u], GICD_TARGET_CORE0);
	}
for(UINT u=0; u<IRQ_COUNT/16; u++)
	{
	io_write(gicd->CONFIG[u], GICD_CONFIG_LEVEL_TRIGGERED);
	}
io_write(gicd->CTRL, GICD_CTRL_ENABLE_GROUP0);
auto gicc=(gicc_regs_t*)ARM_GICC_BASE;
io_write(gicc->PMR, GICC_PMR_PRIORITY);
io_write(gicc->CTRL, GICC_CTRL_ENABLE);
for(UINT core=0; core<CPU_COUNT; core++)
	s_DisableCount[core]=1;
}

VOID Interrupts::Route(UINT irq, IrqTarget target)
{
auto gicd=(gicd_regs_t*)ARM_GICD_BASE;
UINT reg=irq/4;
UINT id=irq%4;
UINT mask=0xF<<id;
UINT value=(UINT)target<<id;
io_write(gicd->TARGET[reg], mask, value);
}

VOID Interrupts::Send(UINT irq, IrqTarget target)
{
auto gicd=(gicd_regs_t*)ARM_GICD_BASE;
UINT value=0;
bits_set(value, GICD_SGIR_CPU_TARGET_LIST, (UINT)target);
bits_set(value, irq);
io_write(gicd->SGIR, value);
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