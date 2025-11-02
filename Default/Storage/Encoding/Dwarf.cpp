//===========
// Dwarf.cpp
//===========

// DWARF was orginally developed by Bell Labs,
// licensed under the GNU Free Documentation Standard

#include "pch.h"


//=======
// Using
//=======

#include "Dwarf.h"
#include "FlagHelper.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Encoding {


//========
// Common
//========

UINT Dwarf::GetEncodedSize(BYTE encoding)
{
UINT size=0;
BYTE type=encoding&0x0F;
switch(type)
	{
	case DW_UNSIGNED:
	case DW_SIGNED:
		break;
	case DW_WORD:
	case DW_SHORT:
		size=2;
		break;
	case DW_DWORD:
	case DW_INT:
		size=4;
		break;
	case DW_POINTER_ABS:
	case DW_QWORD:
	case DW_LONG:
		size=8;
		break;
	default:
		throw InvalidArgumentException();
	}
return size;
}

UINT64 Dwarf::Read(BYTE const*& dwarf)
{
BYTE encoding=*dwarf++;
return ReadEncoded(dwarf, encoding);
}

UINT64 Dwarf::ReadEncoded(BYTE const*& dwarf, BYTE encoding, UINT64 data_rel)
{
SIZE_T ptr_rel=(SIZE_T)dwarf;
SIZE_T value=0;
UINT type=encoding&0xF;
if(type==0xF)
	return 0;
switch(type)
	{
	case DW_POINTER_ABS:
		value=ReadValue<SIZE_T>(dwarf);
		break;
	case DW_UNSIGNED:
		value=ReadUnsigned(dwarf);
		break;
	case DW_WORD:
		value=ReadValue<WORD>(dwarf);
		break;
	case DW_DWORD:
		value=ReadValue<DWORD>(dwarf);
		break;
	case DW_QWORD:
		value=ReadValue<QWORD>(dwarf);
		break;
	case DW_SIGNED:
		value=ReadSigned(dwarf);
		break;
	case DW_SHORT:
		value=ReadValue<SHORT>(dwarf);
		break;
	case DW_INT:
		value=ReadValue<INT>(dwarf);
		break;
	case DW_LONG:
		value=ReadValue<INT64>(dwarf);
		break;
	default:
		throw InvalidArgumentException();
	}
UINT mode=encoding&0x70;
switch(mode)
	{
	case DW_POINTER_ABS:
		break;
	case DW_POINTER_REL:
		{
		value+=ptr_rel;
		break;
		}
	case DW_DATA_REL:
		{
		value+=data_rel;
		break;
		}
	default:
		throw InvalidArgumentException();
	}
if(FlagHelper::Get(encoding, DW_INDIRECT))
	value=*(SIZE_T*)value;
return value;
}

INT64 Dwarf::ReadSigned(BYTE const*& dwarf)
{
UINT64 value=0;
UINT shift=0;
BYTE byte;
do
	{
	byte=*dwarf++;
	value|=((UINT64)byte&0x7F)<<shift;
	shift+=7;
	}
while(byte&0x80);
if(byte&0x40)
	value|=(UINT64_MAX)<<shift;
return (INT64)value;
}

UINT64 Dwarf::ReadUnsigned(BYTE const*& dwarf)
{
UINT64 value=0;
UINT shift=0;
BYTE byte;
do
	{
	byte=*dwarf++;
	value|=((UINT64)byte&0x7F)<<shift;
	shift+=7;
	}
while(byte&0x80);
return value;
}

UINT Dwarf::WriteSigned(OutputStream* dwarf, INT64 ivalue)
{
BOOL neg=false;
if(ivalue<0)
	{
	ivalue=-ivalue;
	neg=true;
	}
UINT64 value=(UINT64)ivalue;
UINT size=0;
do
	{
	BYTE byte=(BYTE)value&0x7F;
	value>>=7;
	if(value)
		{
		byte|=0x80;
		}
	else if(neg)
		{
		if(byte&0x40)
			{
			value=0x80;
			byte|=0x80;
			}
		else
			{
			byte|=0x40;
			}
		}
	if(dwarf)
		{
		size+=(UINT)dwarf->Write(&byte, 1);
		}
	else
		{
		size++;
		}
	}
while(value);
return size;
}

}}