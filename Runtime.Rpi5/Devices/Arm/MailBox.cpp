//=============
// MailBox.cpp
//=============

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include "Devices/System/Memory.h"
#include "BitHelper.h"
#include "MailBox.h"

using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Arm {


//===========
// Registers
//===========

typedef struct
{
REG READ;
REG RES0[5];
REG STATUS;
REG RES1;
REG WRITE;
}MAILBOX_REGS;

namespace STATUS
{
constexpr UINT FULL=(1<<31);
constexpr UINT EMPTY=(1<<30);
}


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

BYTE PropertyBuffer[PROPERTY_BUFSIZE] ALIGN(16);

PROPERTY_BUFFER* Properties=(PROPERTY_BUFFER*)PropertyBuffer;


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
auto buf=Memory::Uncached(Properties);
buf->Size=buf_size;
buf->Code=PROPERTY_REQUEST;
buf->Tag=tag;
buf->BufferSize=prop_size;
buf->Status=0;
MemoryHelper::Copy(buf->Data, prop, prop_size);
MemoryHelper::Fill(&buf->Data[prop_size], sizeof(UINT), 0);
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
auto mbox=(MAILBOX_REGS*)ARM_MAILBOX_BASE;
addr&=0xFFFFFFF0;
addr|=(UINT)ch;
while(1)
	{
	while(BitHelper::Get(mbox->STATUS, STATUS::EMPTY));
	if(BitHelper::Get(mbox->READ)==addr)
		return true;
	}
return false;
}

VOID MailBox::Write(MailBoxChannel ch, UINT addr)
{
auto mbox=(MAILBOX_REGS*)ARM_MAILBOX_BASE;
addr&=0xFFFFFFF0;
addr|=(UINT)ch;
while(BitHelper::Get(mbox->STATUS, STATUS::FULL));
BitHelper::Write(mbox->WRITE, addr);
}

}}
