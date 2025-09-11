//==================
// StringBuffer.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/StringBuffer.h"

using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace Storage {


//==========
// Settings
//==========

#ifdef _UNICODE
constexpr StreamFormat StringStreamFormat=StreamFormat::Unicode;
#else
constexpr StreamFormat StringStreamFormat=StreamFormat::Ansi;
#endif


//==============
// Input-Stream
//==============

SIZE_T StringBuffer::Read(VOID* buf, SIZE_T size)
{
SIZE_T copy=TypeHelper::Min(size, m_Size-m_Position);
if(!copy)
	return 0;
auto str=(BYTE const*)m_Value->Begin();
MemoryHelper::Copy(buf, &str[m_Position], copy);
m_Position+=copy;
return copy;
}


//==========================
// Con-/Destructors Private
//==========================

StringBuffer::StringBuffer(Handle<String> value):
Stream(StringStreamFormat),
m_Position(0),
m_Size(0),
m_Value(value)
{
if(m_Value)
	{
	UINT len=m_Value->GetLength();
	m_Size=(len+1)*sizeof(TCHAR);
	}
}

}