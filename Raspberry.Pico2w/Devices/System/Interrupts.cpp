//================
// Interrupts.cpp
//================

#include "Interrupts.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/System/Cpu.h"
#include "Exception.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//===========
// Registers
//===========

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
RW32 PRIO[16];
}NVIC_REGS;

const UINT ICSR_BASE		=PPB_BASE+0xED04;
const UINT ICSR_PENDSVSET	=(1<<28);
const UINT ICSR_PENDSVCLR	=(1<<27);


//=========
// Handler
//=========

extern "C" VOID HandleInterrupt(UINT irq)
{
UINT reg=irq/32;
UINT mask=1U<<(irq%32);
auto ic=(NVIC_REGS*)NVIC_BASE;
IoHelper::Write(ic->CLRPEND[reg], mask);
Interrupts::HandleInterrupt(irq);
}

extern "C" VOID HandleTaskSwitch()
{
auto icsr=(RW32*)ICSR_BASE;
*icsr|=ICSR_PENDSVCLR;
Interrupts::HandleTaskSwitch();
}


//========
// Common
//========

BOOL Interrupts::Active()noexcept
{
UINT core=Cpu::GetId();
return s_Active[core];
}

VOID Interrupts::Disable()noexcept
{
UINT core=Cpu::GetId();
if(++s_DisableCount[core]==1)
	Cpu::DisableInterrupts();
}

VOID Interrupts::Disable(Irq irq)
{
auto ic=(NVIC_REGS*)NVIC_BASE;
UINT reg=(UINT)irq/32;
UINT mask=1U<<((UINT)irq%32);
IoHelper::Set(ic->CLREN[reg], mask);
}

VOID Interrupts::Enable()noexcept
{
UINT core=Cpu::GetId();
if(s_DisableCount[core]==0)
	return;
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

VOID Interrupts::HandleInterrupt(UINT irq)noexcept
{
UINT core=Cpu::GetId();
s_Active[core]=true;
s_DisableCount[core]++;
if(s_IrqHandler[irq])
	s_IrqHandler[irq]->Run();
s_Active[core]=false;
s_DisableCount[core]--;
}

VOID Interrupts::HandleTaskSwitch()noexcept
{
UINT core=Cpu::GetId();
s_Active[core]=true;
s_DisableCount[core]++;
Scheduler::HandleTaskSwitch();
s_Active[core]=false;
s_DisableCount[core]--;
}

VOID Interrupts::Initialize()noexcept
{
for(UINT core=0; core<Cpu::CPU_COUNT; core++)
	s_DisableCount[core]=1;
}

VOID Interrupts::Send(Irq irq, UINT core)
{
assert(irq==Irq::TaskSwitch);
UINT current=Cpu::GetId();
if(core==current)
	{
	auto icsr=(RW32*)ICSR_BASE;
	*icsr|=ICSR_PENDSVSET;
	}
else
	{
	throw NotImplementedException();
	}
}

VOID Interrupts::SetHandler(Irq irq, VOID (*proc)())
{
if(proc)
	{
	auto handler=new InterruptProcedure(proc);
	SetHandlerInternal(irq, handler);
	}
else
	{
	SetHandlerInternal(irq, nullptr);
	}
}


//================
// Common Private
//================

VOID Interrupts::SetHandlerInternal(Irq irq, InterruptHandler* handler)noexcept
{
UINT id=(UINT)irq;
auto old_handler=s_IrqHandler[id];
s_IrqHandler[id]=handler;
if(old_handler)
	delete old_handler;
handler? Enable(irq): Disable(irq);
}

BOOL Interrupts::s_Active[Cpu::CPU_COUNT]={ false };
UINT Interrupts::s_DisableCount[Cpu::CPU_COUNT]={ 0 };
InterruptHandler* Interrupts::s_IrqHandler[Interrupts::IRQ_COUNT]={ nullptr };

}}
