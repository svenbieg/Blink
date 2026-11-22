//=========
// Cpu.cpp
//=========

#include "Cpu.h"


//=======
// Using
//=======

#include "Concurrency/SpinLock.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/MailBox.h"
#include "Devices/System/Sio.h"
#include "Devices/Peripherals.h"
#include <config.h>

using namespace Concurrency;
using namespace Runtime;

extern SIZE_T __stack_end;

extern "C" VOID isr_reset();


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//========
// Common
//========

BOOL Cpu::CompareAndSet(volatile UINT* value_ptr, UINT cmp, UINT set) noexcept
{
auto sio=(SIO_REGS*)SIO_BASE;
SIZE_T addr=(SIZE_T)value_ptr;
UINT lock=(UINT)(addr%32);
while(IoHelper::Read(sio->SPINLOCK[lock])==0);
DataMemoryBarrier();
BOOL success=false;
if(*value_ptr==cmp)
	{
	*value_ptr=set;
	success=true;
	}
DataMemoryBarrier();
IoHelper::Write(sio->SPINLOCK[lock], 1);
return success;
}

VOID Cpu::Delay(UINT cycles)noexcept
{
__asm inline volatile("\
loop%=:\n\
subs %0, %0, #1\n\
bne loop%=\n\
":: "r" (cycles));
}

UINT Cpu::GetId()noexcept
{
auto sio=(SIO_REGS*)SIO_BASE;
return sio->CPUID;
}

UINT Cpu::InterlockedDecrement(volatile UINT* value_ptr)noexcept
{
auto sio=(SIO_REGS*)SIO_BASE;
SIZE_T addr=(SIZE_T)value_ptr;
UINT lock=(UINT)(addr%32);
while(IoHelper::Read(sio->SPINLOCK[lock])==0);
DataMemoryBarrier();
UINT value=*value_ptr;
value--;
*value_ptr=value;
DataMemoryBarrier();
IoHelper::Write(sio->SPINLOCK[lock], 1);
return value;
}

UINT Cpu::InterlockedIncrement(volatile UINT* value_ptr)noexcept
{
auto sio=(SIO_REGS*)SIO_BASE;
SIZE_T addr=(SIZE_T)value_ptr;
UINT lock=(UINT)(addr%32);
while(IoHelper::Read(sio->SPINLOCK[lock])==0);
DataMemoryBarrier();
UINT value=*value_ptr;
value++;
*value_ptr=value;
DataMemoryBarrier();
IoHelper::Write(sio->SPINLOCK[lock], 1);
return value;
}

VOID Cpu::ResumeAll()noexcept
{
UINT id=GetId();
UINT other=id? 0: 1;
s_Suspension[other]=Suspension::None;
Cpu::SetEvent();
}

VOID Cpu::SetContext(VOID (*task_proc)(VOID*), VOID* param, SIZE_T stack)noexcept
{
__asm inline volatile("\
mov lr, %0\n\
mov r0, %1\n\
mov sp, %2\n\
bx lr":: "r" (task_proc), "r" (param), "r" (stack): "r0");
}

VOID Cpu::Suspend()noexcept
{
UINT id=GetId();
s_Suspension[id]=Suspension::Suspended;
Cpu::SetEvent();
while(s_Suspension[id]==Suspension::Suspended)
	Cpu::WaitForEvent();
}

VOID Cpu::SuspendAll()noexcept
{
UINT id=GetId();
UINT other=id? 0: 1;
Interrupts::Send(Irq::SuspendCore, other);
while(s_Suspension[other]!=Suspension::Suspended)
	Cpu::WaitForEvent();
}

VOID Cpu::WakeupSecondary()
{
const UINT CPU_ID=1;
SIZE_T stack_top=(SIZE_T)&__stack_end-CPU_ID*CONFIG_STACK_SIZE;
const UINT VTOR=XIP_BASE;
const UINT THUMB_BIT=1;
const UINT CMD_SEQ[]={ 0, 0, 1, VTOR, stack_top, (SIZE_T)&isr_reset|THUMB_BIT };
const UINT CMD_COUNT=TypeHelper::ArraySize(CMD_SEQ);
for(UINT u=0; u<CMD_COUNT; u++)
	{
	if(CMD_SEQ[u]==0)
		MailBox::Clear();
	MailBox::Write(CMD_SEQ[u]);
	if(MailBox::Read()!=CMD_SEQ[u])
		throw DeviceNotReadyException();
	}
}


//====================
// Suspension Private
//====================

volatile Cpu::Suspension Cpu::s_Suspension[CPU_COUNT]={ Suspension::None };

}}