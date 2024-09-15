//=============
// MailBox.cpp
//=============

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/Peripherals.h"
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
REG RESERVED[5];
REG STATUS;
REG CONFIG;
REG WRITE;
}MAILBOX_REGS;

MAILBOX_REGS* MAILBOX=(MAILBOX_REGS*)ARM_MAILBOX_BASE;


//========
// Status
//========

typedef enum
{
STATUS_FULL=1<<31,
STATUS_EMPTY=1<<30,
}MAILBOX_STATUS;


//=================
// Property-Buffer
//=================

typedef struct
{
UINT Size;
UINT Code;
PropertyTag Tag;
UINT BufferSize;
UINT ValueLength;
BYTE Data[0];
}PROPERTY_BUFFER;

constexpr UINT PROPERTY_END=0;
constexpr UINT PROPERTY_REQUEST=0;

constexpr UINT MAILBOX_BUFSIZE=64;

BYTE PropertyBuffer[MAILBOX_BUFSIZE] ALIGN(16);

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

VOID MailBox::GetProperty(PropertyTag tag, VOID* prop, UINT prop_size)
{
UINT buf_size=prop_size+sizeof(PROPERTY_BUFFER);
assert(buf_size<=MAILBOX_BUFSIZE);
Properties->Size=buf_size;
Properties->Code=PROPERTY_REQUEST;
Properties->Tag=tag;
Properties->BufferSize=prop_size;
Properties->ValueLength=0;
CopyMemory(Properties->Data, prop, prop_size);
CopyMemory(&Properties->Data[prop_size], &PROPERTY_END, sizeof(UINT));
Write(MailBoxChannel::Properties, (UINT)(SIZE_T)Properties);
Read(MailBoxChannel::Properties);
CopyMemory(prop, &Properties->Data, prop_size);
}


//================
// Common Private
//================

UINT MailBox::Read(MailBoxChannel ch)
{
UINT value=0;
while(1)
	{
	while(Bits::Get(MAILBOX->STATUS, STATUS_EMPTY));
	value=Bits::Get(MAILBOX->READ);
	MailBoxChannel dst=(MailBoxChannel)(value&0xF);
	value&=0xFFFFFFF0;
	if(dst==ch)
		break;
	}
return value;
}

VOID MailBox::Write(MailBoxChannel ch, UINT value)
{
value&=0xFFFFFFF0;
value|=(UINT)ch;
while(Bits::Get(MAILBOX->STATUS, STATUS_FULL));
Bits::Write(MAILBOX->WRITE, value);
}

}}
