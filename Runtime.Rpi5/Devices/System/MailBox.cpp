//=============
// MailBox.cpp
//=============

#include "MailBox.h"


//=======
// Using
//=======

#include "Devices/System/Memory.h"
#include "Devices/IoHelper.h"
#include "MemoryHelper.h"
#include <assert.h>
#include <attr.h>
#include <base.h>

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
RW32 READ;
RW32 RES0[5];
RW32 STATUS;
RW32 RES1;
RW32 WRITE;
}mailbox_regs_t;

constexpr UINT STATUS_FULL=(1<<31);
constexpr UINT STATUS_EMPTY=(1<<30);


//=================
// Property-Buffer
//=================

typedef struct
{
UINT Size;
RW32 Code;
PropertyTag Tag;
RW32 BufferSize;
RW32 Status;
}PROP_HEADER;

__align(16) typedef struct
{
PROP_HEADER Header;
BYTE Data[44];
}PROP_BUF;

constexpr UINT PROPERTY_REQUEST=0;

PROP_BUF g_PropertyBuffer;


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
UINT buf_size=sizeof(PROP_HEADER)+prop_size+sizeof(UINT);
assert(buf_size<=sizeof(PROP_BUF));
auto buf=Memory::Uncached(&g_PropertyBuffer);
buf->Header.Size=buf_size;
buf->Header.Code=PROPERTY_REQUEST;
buf->Header.Tag=tag;
buf->Header.BufferSize=prop_size;
buf->Header.Status=0;
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
	while(IoHelper::Read(mailbox->STATUS, STATUS_EMPTY));
	if(IoHelper::Read(mailbox->READ)==addr)
		return true;
	}
return false;
}

VOID MailBox::Write(MailBoxChannel ch, UINT addr)
{
auto mailbox=(mailbox_regs_t*)ARM_MAILBOX_BASE;
addr&=0xFFFFFFF0;
addr|=(UINT)ch;
while(IoHelper::Read(mailbox->STATUS, STATUS_FULL));
IoHelper::Write(mailbox->WRITE, addr);
}

}}