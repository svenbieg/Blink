//================
// StringView.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "StringView.h"

using namespace Storage::Streams;


//==============
// Input-Stream
//==============

SIZE_T StringView::Read(VOID* buf, SIZE_T size)
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

StringView::StringView(Handle<String> value):
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
