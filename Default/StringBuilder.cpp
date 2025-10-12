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
m_AppendAnsi(&StringBuilder::BufferAppendAnsi),
m_AppendUnicode(&StringBuilder::BufferAppendUnicode),
m_Buffer(64*sizeof(TCHAR)),
m_Position(0),
m_Size(0),
m_ToString(&StringBuilder::BufferToString)
{
if(len)
	Initialize(len);
}


//========
// Common
//========

UINT StringBuilder::Append(CHAR c)
{
return (this->*m_AppendAnsi)(c);
}

UINT StringBuilder::Append(WCHAR c)
{
return (this->*m_AppendUnicode)(c);
}

UINT StringBuilder::Append(LPCSTR String)
{
UINT pos=0;
for(; String[pos]; pos++)
	(this->*m_AppendAnsi)(String[pos]);
return pos;
}

UINT StringBuilder::Append(LPCWSTR String)
{
UINT pos=0;
for(; String[pos]; pos++)
	(this->*m_AppendUnicode)(String[pos]);
return pos;
}

UINT StringBuilder::Append(UINT Length, LPCSTR String)
{
UINT pos=0;
for(; String[pos]; pos++)
	{
	if(pos==Length)
		break;
	(this->*m_AppendAnsi)(String[pos]);
	}
return pos;
}

UINT StringBuilder::Append(UINT Length, LPCWSTR String)
{
UINT pos=0;
for(; String[pos]; pos++)
	{
	if(pos==Length)
		break;
	(this->*m_AppendUnicode)(String[pos]);
	}
return pos;
}

VOID StringBuilder::Initialize(UINT len)
{
m_Buffer.clear();
m_Position=0;
if(len==0)
	{
	m_AppendAnsi=&StringBuilder::StringAppendAnsi;
	m_AppendUnicode=&StringBuilder::StringAppendUnicode;
	m_Size=0;
	m_String=nullptr;
	m_ToString=&StringBuilder::StringToString;
	}
else
	{
	m_AppendAnsi=&StringBuilder::StringAppendAnsi;
	m_AppendUnicode=&StringBuilder::StringAppendUnicode;
	m_Size=len+1;
	m_String=String::Create(len, nullptr);
	m_ToString=&StringBuilder::StringToString;
	}
}

Handle<String> StringBuilder::ToString()
{
return (this->*m_ToString)();
}


//================
// Common Private
//================

UINT StringBuilder::BufferAppendAnsi(CHAR c)
{
TCHAR tc=CharHelper::ToChar(c);
m_Buffer.write(&tc, sizeof(TCHAR));
return 1;
}

UINT StringBuilder::BufferAppendUnicode(WCHAR c)
{
TCHAR tc=CharHelper::ToChar(c);
m_Buffer.write(&tc, sizeof(TCHAR));
return 1;
}

Handle<String> StringBuilder::BufferToString()
{
m_Buffer.flush();
UINT len=m_Buffer.available()/sizeof(TCHAR);
if(len==0)
	return nullptr;
auto str=String::Create(len, nullptr);
auto buf=const_cast<LPTSTR>(str->Begin());
m_Buffer.read(buf, len*sizeof(TCHAR));
buf[len]=0;
str->m_Hash=StringHelper::GetHash(buf);
str->m_Length=len;
return str;
}

UINT StringBuilder::StringAppendAnsi(CHAR c)
{
if(m_Position+1>=m_Size)
	throw BufferOverrunException();
auto buf=const_cast<LPTSTR>(m_String->Begin());
buf[m_Position++]=CharHelper::ToChar(c);
return 1;
}

UINT StringBuilder::StringAppendUnicode(WCHAR c)
{
if(m_Position+1>=m_Size)
	throw BufferOverrunException();
auto buf=const_cast<LPTSTR>(m_String->Begin());
buf[m_Position++]=CharHelper::ToChar(c);
return 1;
}

Handle<String> StringBuilder::StringToString()
{
if(m_Position+1!=m_Size)
	throw InvalidArgumentException();
auto str=m_String;
auto buf=const_cast<LPTSTR>(str->Begin());
buf[m_Position]=0;
str->m_Hash=StringHelper::GetHash(buf);
str->m_Length=m_Position;
Initialize(0);
return str;
}
