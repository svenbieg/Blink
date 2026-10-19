//================
// Interrupts.cpp
//================

#include "Interrupts.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/MailBox.h"
#include "Devices/System/Sio.h"
#include "Devices/IoHelper.h"
#include "Devices/Peripherals.h"
#include "Exception.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//======================
// Interrupt Controller
//======================

typedef struct
{
RW32 SETEN[2];
UINT RES0[30];
RW32 CLREN[2];
UINT RES1[30];
RW32 SETPEND[2];
UINT RES2[30];
RW32 CLRPEND[2];
UINT RES3[30];
RW32 ACTIVE[2];
UINT RES4[30];
RW32 ITNS[2];
UINT RES5[30];
RW08 PRIO[64];
}NVIC_REGS;


//======================
// System Control Block
//======================

typedef struct
{
RW32 CPUID;
RW32 ICSR;
RW32 VTOR;
RW32 AIRCR;
RW32 SCR;
RW32 CCR;
RW08 SHPR[12];
RW32 SHCSR;
}SCB_REGS;

const UINT ICSR_PENDSVSET	=(1<<28);
const UINT ICSR_PENDSVCLR	=(1<<27);


//=========
// Handler
//=========

extern "C" VOID HandleInterrupt(UINT irq)
{
Interrupts::HandleInterrupt(irq);
}

extern "C" VOID HandleTaskSwitch()
{
Interrupts::HandleTaskSwitch();
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

VOID Interrupts::Disable(Irq irq)
{
auto ic=(NVIC_REGS*)NVIC_BASE;
UINT reg=(UINT)irq/32;
UINT mask=1U<<((UINT)irq%32);
IoHelper::Set(ic->CLREN[reg], mask);
}

BOOL Interrupts::Disabled()noexcept
{
UINT core=Cpu::GetId();
return s_DisableCount[core]>0;
}

VOID Interrupts::Enable()noexcept
{
UINT core=Cpu::GetId();
assert(s_DisableCount[core]>=1);
if(--s_DisableCount[core]==0)
	Cpu::EnableInterrupts();
}

VOID Interrupts::Enable(Irq irq)
{
auto ic=(NVIC_REGS*)NVIC_BASE;
UINT reg=(UINT)irq/32;
UINT mask=1U<<((UINT)irq%32);
IoHelper::Write(ic->CLRPEND[reg], mask);
IoHelper::Write(ic->SETEN[reg], mask);
}

BOOL Interrupts::Enabled()noexcept
{
UINT core=Cpu::GetId();
return s_DisableCount[core]==0;
}

VOID Interrupts::HandleInterrupt(UINT irq)noexcept
{
UINT core=Cpu::GetId();
TaskMonitor::SetInterrupt(core);
s_DisableCount[core]++;
SpinLock lock(s_CriticalSection);
auto handler=s_IrqHandler[irq];
lock.Unlock();
if(handler)
	handler->Call();
TaskMonitor::ClearInterrupt(core);
s_DisableCount[core]--;
}

VOID Interrupts::HandleTaskSwitch()noexcept
{
UINT core=Cpu::GetId();
TaskMonitor::SetInterrupt(core);
s_DisableCount[core]++;
Scheduler::HandleTaskSwitch();
TaskMonitor::ClearInterrupt(core);
s_DisableCount[core]--;
}

VOID Interrupts::Initialize()noexcept
{
SetInterruptPriority();
for(UINT core=0; core<Cpu::CPU_COUNT; core++)
	s_DisableCount[core]=1;
SetHandler(Irq::SioFifo, &Interrupts::HandleMailBox);
}

VOID Interrupts::InitializeSecondary()noexcept
{
SetInterruptPriority();
Enable(Irq::SioFifo);
}

BOOL Interrupts::IsEnabled(Irq irq)noexcept
{
auto ic=(NVIC_REGS*)NVIC_BASE;
UINT reg=(UINT)irq/32;
UINT mask=1U<<((UINT)irq%32);
UINT status=IoHelper::Read(ic->SETEN[reg], mask);
return status!=0;
}

VOID Interrupts::Send(Irq irq, UINT core)
{
assert(irq==Irq::TaskSwitch);
UINT current=Cpu::GetId();
if(core==current)
	{
	auto scb=(SCB_REGS*)SCB_BASE;
	IoHelper::Set(scb->ICSR, ICSR_PENDSVSET);
	}
else
	{
	MailBox::Write(CMD_TASK_SWITCH);
	}
}

VOID Interrupts::SetHandler(Irq irq, nullptr_t)
{
SetHandlerInternal(irq, nullptr);
}

VOID Interrupts::SetHandler(Irq irq, VOID (*proc)())
{
auto handler=CallableFunction<VOID>::Create(proc);
SetHandlerInternal(irq, handler);
}


//================
// Common Private
//================

VOID Interrupts::HandleMailBox()noexcept
{
UINT cmd=0;
if(MailBox::TryRead(&cmd))
	{
	if(cmd==CMD_TASK_SWITCH)
		{
		auto scb=(SCB_REGS*)SCB_BASE;
		IoHelper::Set(scb->ICSR, ICSR_PENDSVSET);
		}
	}
auto sio=(SIO_REGS*)SIO_BASE;
IoHelper::Write(sio->FIFO_ST, 0xFF);
}

VOID Interrupts::SetHandlerInternal(Irq irq, InterruptHandler* handler)noexcept
{
if(!handler)
	Disable(irq);
UINT id=(UINT)irq;
SpinLock lock(s_CriticalSection);
auto old_handler=s_IrqHandler[id];
s_IrqHandler[id]=handler;
lock.Unlock();
if(handler)
	Enable(irq);
}

VOID Interrupts::SetInterruptPriority()
{
auto scb=(SCB_REGS*)SCB_BASE;
static constexpr UINT PEND_SV=(UINT)SystemException::PendSV-3;
static constexpr BYTE LOWEST_PRIORITY=0xF0;
IoHelper::Write(scb->SHPR[PEND_SV], LOWEST_PRIORITY);
auto ic=(NVIC_REGS*)NVIC_BASE;
for(UINT u=0; u<IRQ_COUNT; u++)
	IoHelper::Write(ic->PRIO[u], LOWEST_PRIORITY);
}

CriticalSection Interrupts::s_CriticalSection;
UINT Interrupts::s_DisableCount[Cpu::CPU_COUNT]={ 0 };
Handle<InterruptHandler> Interrupts::s_IrqHandler[Interrupts::IRQ_COUNT];

}}
