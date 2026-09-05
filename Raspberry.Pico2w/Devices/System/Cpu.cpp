//=========
// Cpu.cpp
//=========

#include "Cpu.h"


//=======
// Using
//=======

#include "Devices/System/Interrupts.h"
#include "Devices/System/MailBox.h"
#include "Devices/System/Sio.h"
#include "Devices/Peripherals.h"
#include "Runtime/Configuration.h"

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

BOOL Cpu::CompareAndSet(volatile UINT* value_ptr, UINT cmp, UINT set)noexcept
{
UINT read=0;
UINT result=1;
__asm inline volatile("\
ldaex %0, [%2]\n\
cmp %0, %3\n\
bne failed%=\n\
stlex %1, %4, [%2]\n\
b done%=\n\
failed%=:\n\
clrex\n\
done%=:\n\
": "=&r" (read), "=&r" (result): "r" (value_ptr), "r" (cmp), "r" (set): "memory");
return result==0;
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
UINT value=0;
UINT result=0;
__asm inline volatile("\
%=:\n\
ldaex %0, [%2]\n\
sub %0, %0, #1\n\
stlex %1, %0, [%2]\n\
cmp %1, #0\n\
bne %=b\n\
": "=&r" (value), "=&r" (result): "r" (value_ptr): "memory");
return value;
}

UINT Cpu::InterlockedIncrement(volatile UINT* value_ptr)noexcept
{
UINT value=0;
UINT result=0;
__asm inline volatile("\
%=:\n\
ldaex %0, [%2]\n\
add %0, %0, #1\n\
stlex %1, %0, [%2]\n\
cmp %1, #0\n\
bne %=b\n\
": "=&r" (value), "=&r" (result): "r" (value_ptr): "memory");
return value;
}

VOID Cpu::SetContext(VOID (*task_proc)(VOID*), VOID* param, SIZE_T stack)noexcept
{
__asm inline volatile("\
mov lr, %0\n\
mov r0, %1\n\
mov sp, %2\n\
bx lr":: "r" (task_proc), "r" (param), "r" (stack): "r0");
}

VOID Cpu::WakeupSecondary()
{
SIZE_T stack_top=(SIZE_T)&__stack_end-CONFIG_STACK_SIZE;
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

}}