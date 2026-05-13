//=========
// Cpu.cpp
//=========

#include "Cpu.h"


//=======
// Using
//=======

#include "Devices/System/Sio.h"
#include "Devices/Peripherals.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//========
// Common
//========

BOOL Cpu::CompareAndSet(volatile UINT* Value, UINT Compare, UINT Set)noexcept
{
UINT read;
UINT result=1;
__asm inline volatile("\
ldaexb %4, [%1]\n\
cmp %4, %2\n\
bne %=f\n\
strex %0, %3, [%1]\n\
%=:\n\
": "=&r" (result): "r" (Value), "r" (Compare), "r" (Set), "r" (read));
return result==0;
}

VOID Cpu::Delay(UINT Cycles)noexcept
{
__asm inline volatile("\
%=:\n\
subs %[cycles], %[cycles], #1\n\
bne %=b\n\
":: [cycles] "r" (Cycles));
}

UINT Cpu::GetId()noexcept
{
auto sio=(SIO_REGS*)SIO_BASE;
return sio->CPUID;
}

UINT Cpu::InterlockedDecrement(volatile UINT* Value)noexcept
{
UINT value;
UINT result;
__asm inline volatile("\
%=:\n\
ldaexb %0, [%1]\n\
sub %0, %0, #1\n\
strexb %2, %0, [%1]\n\
cmp %2, #0\n\
bne %=b\n\
": "=&r" (value): "r" (Value), "r" (result));
return value;
}

UINT Cpu::InterlockedIncrement(volatile UINT* Value)noexcept
{
UINT value;
UINT result;
__asm inline volatile("\
%=:\n\
ldaexb %0, [%1]\n\
add %0, %0, #1\n\
strexb %2, %0, [%1]\n\
cmp %2, #0\n\
bne %=b\n\
": "=&r" (value): "r" (Value), "r" (result));
return value;
}

VOID Cpu::SetContext(VOID (*TaskProc)(VOID*), VOID* Parameter, SIZE_T StackPointer)noexcept
{
__asm inline volatile("\
mov lr, %0\n\
mov r0, %1\n\
mov sp, %2\n\
bx lr":: "r" (TaskProc), "r" (Parameter), "r" (StackPointer): "r0");
}

}}