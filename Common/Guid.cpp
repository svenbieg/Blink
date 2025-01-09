//==========
// Guid.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "Guid.h"


//========
// Struct
//========

GLOBAL_UNIQUE_ID::GLOBAL_UNIQUE_ID(DWORD data1, WORD data2, WORD data3, QWORD data4)
{
MemoryHelper::Copy(m_Value, &data1, 4);
MemoryHelper::Copy(&m_Value[4], &data2, 2);
MemoryHelper::Copy(&m_Value[6], &data3, 2);
MemoryHelper::Copy(&m_Value[8], &data4, 8);
}

GLOBAL_UNIQUE_ID::GLOBAL_UNIQUE_ID(DWORD data1, WORD data2, WORD data3, BYTE data4, BYTE data5, BYTE data6, BYTE data7, BYTE data8, BYTE data9, BYTE data10, BYTE data11)
{
MemoryHelper::Copy(m_Value, &data1, 4);
MemoryHelper::Copy(&m_Value[4], &data2, 2);
MemoryHelper::Copy(&m_Value[6], &data3, 2);
m_Value[8]=data4;
m_Value[9]=data5;
m_Value[10]=data6;
m_Value[11]=data7;
m_Value[12]=data8;
m_Value[13]=data9;
m_Value[14]=data10;
m_Value[15]=data11;
}

BOOL GLOBAL_UNIQUE_ID::FromString(Handle<String> value, GLOBAL_UNIQUE_ID* id_ptr)
{
if(!value)
	return false;
auto str=value->Begin();
if(CharHelper::Compare(str[0], '{'))
	str++;
UINT data1;
UINT data2;
UINT data3;
UINT64 data4;
if(StringHelper::Scan(str, "%x-%x-%x-%x", &data1, &data2, &data3, &data4)!=4)
	return false;
if(id_ptr)
	*id_ptr=GLOBAL_UNIQUE_ID(data1, (WORD)data2, (WORD)data3, data4);
return true;
}

Handle<String> GLOBAL_UNIQUE_ID::ToString(GLOBAL_UNIQUE_ID const& id)
{
auto data=id.m_Value;
UINT data1=*(DWORD*)&data[0];
UINT data2=*(WORD*)&data[4];
UINT data3=*(WORD*)&data[6];
UINT64 data4=*(QWORD*)&data[8];
CHAR buf[40];
StringHelper::Print(buf, 40, "{%08x-%04x-%04x-%016x}", data1, data2, data3, data4);
return buf;
}


//========
// Access
//========

GLOBAL_UNIQUE_ID Guid::Get()
{
GLOBAL_UNIQUE_ID value=m_Value;
Reading(this, value);
return value;
}

SIZE_T Guid::WriteToStream(OutputStream* Stream)
{
if(!Stream)
	return sizeof(GLOBAL_UNIQUE_ID);
GLOBAL_UNIQUE_ID value=Get();
return Stream->Write(&value, sizeof(GLOBAL_UNIQUE_ID));
}


//==============
// Modification
//==============

BOOL Guid::FromString(Handle<String> value, BOOL notify)
{
GLOBAL_UNIQUE_ID id;
if(!GLOBAL_UNIQUE_ID::FromString(value, &id))
	return false;
return Set(id, notify);
}

SIZE_T Guid::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return sizeof(GLOBAL_UNIQUE_ID);
GLOBAL_UNIQUE_ID id;
SIZE_T size=stream->Read(&id, sizeof(GLOBAL_UNIQUE_ID));
if(size==sizeof(GLOBAL_UNIQUE_ID))
	Set(id, notify);
return size;
}

BOOL Guid::Set(GLOBAL_UNIQUE_ID const& value, BOOL notify)
{
if(m_Value==value)
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
}
