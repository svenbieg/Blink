//=============
// MailBox.cpp
//=============

#include "MailBox.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Devices/System/Sio.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//========
// Common
//========

VOID MailBox::Clear()
{
auto sio=(SIO_REGS*)SIO_BASE;
for(UINT retry=0; retry>=0; retry++)
	{
	if(IoHelper::Read(sio->FIFO_ST, FIFO_ST_VALID)==0)
		break;
	IoHelper::Read(sio->FIFO_RD);
	if(retry>=10)
		throw DeviceNotReadyException();
	}
}

UINT MailBox::Read()
{
auto sio=(SIO_REGS*)SIO_BASE;
IoHelper::Retry(sio->FIFO_ST, FIFO_ST_VALID, FIFO_ST_VALID);
return IoHelper::Read(sio->FIFO_RD);
}

BOOL MailBox::TryRead(UINT* value_ptr)
{
auto sio=(SIO_REGS*)SIO_BASE;
if(IoHelper::Read(sio->FIFO_ST, FIFO_ST_VALID))
	{
	*value_ptr=IoHelper::Read(sio->FIFO_RD);
	return true;
	}
return false;
}

VOID MailBox::Write(UINT value)
{
auto sio=(SIO_REGS*)SIO_BASE;
assert(IoHelper::Read(sio->FIFO_ST, FIFO_ST_RDY));
IoHelper::Write(sio->FIFO_WR, value);
Cpu::SetEvent();
}

}}