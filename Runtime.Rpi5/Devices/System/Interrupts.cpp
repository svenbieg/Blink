//================
// Interrupts.cpp
//================

#include "Interrupts.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Devices/IoHelper.h"
#include <assert.h>
#include <base.h>

using namespace Concurrency;


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
RW32 CTRL;
RW32 RES0[31];
RW32 GROUP[32];
RW32 SET_ENABLED[32];
RW32 CLEAR_ENABLED[32];
RW32 SET_PENDING[32];
RW32 CLEAR_PENDING[32];
RW32 SET_ACTIVE[32];
RW32 CLEAR_ACTIVE[32];
RW32 PRIORITY[256];
RW32 TARGET[256];
RW32 CONFIG[192];
RW32 SGIR;
}GICD_REGS;

constexpr UINT GICD_CTRL_DISABLE			=0;
constexpr UINT GICD_CTRL_ENABLE_GROUP0		=(1<<0);
constexpr UINT GICD_CTRL_ENABLE_GROUP1		=(1<<1);
constexpr UINT GICD_PRIORITY_DEFAULT		=0xA0A0A0A0;
constexpr UINT GICD_TARGET_CORE0			=0x01010101;
constexpr UINT GICD_CONFIG_LEVEL_TRIGGERED	=0;
constexpr BITS GICD_SGIR_CPU_TARGET_LIST	={ 0xF, 16 };


//===================
// GIC CPU-Interface
//===================

typedef struct
{
RW32 CTRL;
RW32 PMR;
RW32 RES0;
RW32 IAR;
RW32 EOIR;
}GICC_REGS;

constexpr UINT GICC_CTRL_ENABLE		=1;
constexpr UINT GICC_PMR_PRIORITY	=0xF0;
constexpr UINT GICC_IAR_IRQ_MASK	=0x3FF;


//=============
// IRQ-Handler
//=============

extern "C" VOID HandleInterrupt()
{
auto gicc=(GICC_REGS*)ARM_GICC_BASE;
UINT iar=IoHelper::Read(gicc->IAR);
UINT irq=BitHelper::Get(iar, GICC_IAR_IRQ_MASK);
assert(irq<IRQ_COUNT);
Interrupts::HandleInterrupt(irq);
IoHelper::Write(gicc->EOIR, iar);
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

VOID Interrupts::Enable()
{
UINT core=Cpu::GetId();
if(core>0)
	throw DeviceNotReadyException();
if(--s_DisableCount[core]==0)
	Cpu::EnableInterrupts();
}

BOOL Interrupts::Enabled()
{
UINT core=Cpu::GetId();
return s_DisableCount[core]==0;
}

VOID Interrupts::HandleInterrupt(UINT irq)
{
SpinLock lock(s_CriticalSection);
if(!s_IrqHandlers[irq])
	return;
UINT core=Cpu::GetId();
s_Active[core]=true;
s_DisableCount[core]++;
s_IrqHandlers[irq](s_IrqParameters[irq]);
s_Active[core]=false;
s_DisableCount[core]--;
}

VOID Interrupts::Initialize()
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
IoHelper::Write(gicd->CTRL, GICD_CTRL_DISABLE);
for(UINT u=0; u<IRQ_COUNT/32; u++)
	{
	IoHelper::Write(gicd->CLEAR_ENABLED[u], 0xFFFFFFFF);
	IoHelper::Write(gicd->CLEAR_PENDING[u], 0xFFFFFFFF);
	IoHelper::Write(gicd->CLEAR_ACTIVE[u], 0xFFFFFFFF);
	}
for(UINT u=0; u<IRQ_COUNT/4; u++)
	{
	IoHelper::Write(gicd->PRIORITY[u], GICD_PRIORITY_DEFAULT);
	IoHelper::Write(gicd->TARGET[u], GICD_TARGET_CORE0);
	}
for(UINT u=0; u<IRQ_COUNT/16; u++)
	{
	IoHelper::Write(gicd->CONFIG[u], GICD_CONFIG_LEVEL_TRIGGERED);
	}
IoHelper::Write(gicd->CTRL, GICD_CTRL_ENABLE_GROUP0);
for(UINT core=0; core<CPU_COUNT; core++)
	s_DisableCount[core]=1;
InitializeSecondary();
}

VOID Interrupts::InitializeSecondary()
{
auto gicc=(GICC_REGS*)ARM_GICC_BASE;
IoHelper::Write(gicc->PMR, GICC_PMR_PRIORITY);
IoHelper::Write(gicc->CTRL, GICC_CTRL_ENABLE);
Enable();
}

VOID Interrupts::Route(Irq irq, IrqTarget target)
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT reg=(UINT)irq/4;
UINT id=(UINT)irq%4;
UINT mask=0xF<<id;
UINT value=(UINT)target<<id;
IoHelper::Set(gicd->TARGET[reg], mask, value);
}

VOID Interrupts::Send(Irq irq, IrqTarget target)
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT value=0;
BitHelper::Set(value, GICD_SGIR_CPU_TARGET_LIST, (UINT)target);
BitHelper::Set(value, (UINT)irq);
IoHelper::Write(gicd->SGIR, value);
}

VOID Interrupts::SetHandler(Irq irq, IRQ_HANDLER handler, VOID* param)
{
UINT id=(UINT)irq;
SpinLock lock(s_CriticalSection);
if(handler)
	{
	s_IrqHandlers[id]=handler;
	s_IrqParameters[id]=param;
	Enable(id);
	}
else
	{
	s_IrqHandlers[id]=nullptr;
	s_IrqParameters[id]=nullptr;
	Disable(id);
	}
}


//================
// Common Private
//================

VOID Interrupts::Disable(UINT irq)
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT reg=irq/32;
UINT mask=1UL<<(irq%32);
IoHelper::Write(gicd->CLEAR_ENABLED[reg], mask);
}

VOID Interrupts::Enable(UINT irq)
{
auto gicd=(GICD_REGS*)ARM_GICD_BASE;
UINT reg=irq/32;
UINT mask=1UL<<(irq%32);
IoHelper::Write(gicd->SET_ENABLED[reg], mask);
}

BOOL Interrupts::s_Active[CPU_COUNT]={ false };
CriticalSection Interrupts::s_CriticalSection;
UINT Interrupts::s_DisableCount[CPU_COUNT]={ 0 };
IRQ_HANDLER Interrupts::s_IrqHandlers[IRQ_COUNT]={ nullptr };
VOID* Interrupts::s_IrqParameters[IRQ_COUNT]={ nullptr };

}}