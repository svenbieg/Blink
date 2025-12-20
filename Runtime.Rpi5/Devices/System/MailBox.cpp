//=============
// MailBox.cpp
//=============

#include "Devices/System/MailBox.h"


//=======
// Using
//=======

#include <assert.h>
#include <attr.h>
#include <base.h>
#include <io.h>
#include "Devices/System/Memory.h"
#include "MemoryHelper.h"

using namespace Devices::System;


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
rw32_t READ;
rw32_t RES0[5];
rw32_t STATUS;
rw32_t RES1;
rw32_t WRITE;
}mailbox_regs_t;

constexpr uint32_t STATUS_FULL=(1<<31);
constexpr uint32_t STATUS_EMPTY=(1<<30);


//=================
// Property-Buffer
//=================

typedef struct
{
UINT Size;
volatile UINT Code;
PropertyTag Tag;
volatile UINT BufferSize;
volatile UINT Status;
BYTE Data[0];
}PROPERTY_BUFFER;

constexpr UINT PROPERTY_BUFSIZE=64;
constexpr UINT PROPERTY_REQUEST=0;

BYTE g_PropertyBuffer[PROPERTY_BUFSIZE] __align(16);

PROPERTY_BUFFER* g_Properties=(PROPERTY_BUFFER*)g_PropertyBuffer;


//============
// Clock-Rate
//============

typedef struct
{
ClockType Type;
UINT ClockRate;
}CLOCK_RATE;


//========
// Common
//========

VOID MailBox::GetArmMemory(ARM_MEMORY* info)
{
GetProperty(PropertyTag::GetArmMemory, info, sizeof(ARM_MEMORY));
}

UINT MailBox::GetClockRate(ClockType clock)
{
CLOCK_RATE info;
info.Type=clock;
info.ClockRate=0;
GetProperty(PropertyTag::GetClockRate, &info, sizeof(CLOCK_RATE));
return info.ClockRate;
}

BOOL MailBox::GetProperty(PropertyTag tag, VOID* prop, UINT prop_size)
{
UINT buf_size=sizeof(PROPERTY_BUFFER)+prop_size+sizeof(UINT);
assert(buf_size<=PROPERTY_BUFSIZE);
auto buf=Memory::Uncached(g_Properties);
buf->Size=buf_size;
buf->Code=PROPERTY_REQUEST;
buf->Tag=tag;
buf->BufferSize=prop_size;
buf->Status=0;
MemoryHelper::Copy(buf->Data, prop, prop_size);
MemoryHelper::Zero(&buf->Data[prop_size], sizeof(UINT));
UINT addr=(UINT)(SIZE_T)buf;
Write(MailBoxChannel::Properties, addr);
if(!Read(MailBoxChannel::Properties, addr))
	return false;
MemoryHelper::Copy(prop, &buf->Data, prop_size);
return true;
}


//================
// Common Private
//================

BOOL MailBox::Read(MailBoxChannel ch, UINT addr)
{
auto mailbox=(mailbox_regs_t*)ARM_MAILBOX_BASE;
addr&=0xFFFFFFF0;
addr|=(UINT)ch;
while(1)
	{
	while(io_read(mailbox->STATUS, STATUS_EMPTY));
	if(io_read(mailbox->READ)==addr)
		return true;
	}
return false;
}

VOID MailBox::Write(MailBoxChannel ch, UINT addr)
{
auto mailbox=(mailbox_regs_t*)ARM_MAILBOX_BASE;
addr&=0xFFFFFFF0;
addr|=(UINT)ch;
while(io_read(mailbox->STATUS, STATUS_FULL));
io_write(mailbox->WRITE, addr);
}

}}