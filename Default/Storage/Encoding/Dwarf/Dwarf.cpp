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
switch(encoding&0x0F)
	{
	case DW_OMIT:
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

SIZE_T Dwarf::Read(BYTE const*& dwarf)
{
BYTE encoding=*dwarf++;
return ReadEncoded(dwarf, encoding);
}

SIZE_T Dwarf::ReadEncoded(BYTE const*& dwarf, BYTE encoding, SIZE_T relative)
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
		value+=relative;
		break;
		}
	default:
		throw InvalidArgumentException();
	}
if(GetFlag(encoding, DW_INDIRECT))
	value=*(SIZE_T*)value;
return value;
}

SIZE_T Dwarf::ReadSigned(BYTE const*& dwarf)
{
SIZE_T value=0;
UINT shift=0;
BYTE byte;
do
	{
	byte=*dwarf++;
	value|=((SIZE_T)byte&0x7F)<<shift;
	shift+=7;
	}
while(byte&0x80);
if(byte&0x40)
	value|=(-1ULL)<<shift;
return value;
}

SIZE_T Dwarf::ReadUnsigned(BYTE const*& dwarf)
{
SIZE_T value=0;
UINT shift=0;
BYTE byte;
do
	{
	byte=*dwarf++;
	value|=((SIZE_T)byte&0x7F)<<shift;
	shift+=7;
	}
while(byte&0x80);
return value;
}

}}