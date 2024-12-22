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

Handle<String> GLOBAL_UNIQUE_ID::ToString(GLOBAL_UNIQUE_ID const& id)
{
auto data=id.m_Value;
auto* p1=(UINT*)&data[0];
auto p2=(WORD*)&data[4];
auto p3=(WORD*)&data[6];
CHAR buf[40];
StringHelper::Print(buf, 40, "{%08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x}", *p1, *p2, *p3, data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
return buf;
}


//==========
// Variable
//==========

Handle<String> Guid::ToString(LanguageCode lng)
{
SharedLock lock(m_Mutex);
return m_Value.ToString();
}
