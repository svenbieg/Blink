//=============
// MailBox.cpp
//=============

#include "MailBox.h"


//=======
// Using
//=======

#include "Devices/System/Sio.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//========
// Common
//========

VOID MailBox::Send(UINT value)
{
auto sio=(SIO_REGS*)SIO_BASE;
assert(IoHelper::Read(sio->FIFO_ST, FIFO_ST_RDY));
IoHelper::Write(sio->FIFO_WR, value);
}

}}