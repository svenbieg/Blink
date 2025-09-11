//===================
// StringBuilder.cpp
//===================

#include "pch.h"


//=======
// Using
//=======

#include "StringBuilder.h"


//==================
// Con-/Destructors
//==================

StringBuilder::StringBuilder(UINT len):
m_Buffer(nullptr),
m_Position(0),
m_Size(len+1)
{
m_String=String::Create(len, nullptr);
m_Buffer=const_cast<LPTSTR>(m_String->Begin());
}


//========
// Common
//========

UINT StringBuilder::Append(CHAR Char)
{
return AppendChar(Char);
}

UINT StringBuilder::Append(WCHAR Char)
{
return AppendChar(Char);
}

UINT StringBuilder::Append(LPCSTR String)
{
return AppendString(UINT_MAX, String);
}

UINT StringBuilder::Append(LPCWSTR String)
{
return AppendString(UINT_MAX, String);
}

UINT StringBuilder::Append(UINT Length, LPCSTR String)
{
return AppendString(UINT_MAX, String);
}

UINT StringBuilder::Append(UINT Length, LPCWSTR String)
{
return AppendString(UINT_MAX, String);
}

Handle<String> StringBuilder::ToString()
{
m_Buffer[m_Position]=0;
m_String->m_Hash=StringHelper::GetHash(m_Buffer);
m_String->m_Length=m_Position;
return m_String;
}
