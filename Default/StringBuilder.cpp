//===================
// StringBuilder.cpp
//===================

#include "StringBuilder.h"


//==================
// Con-/Destructors
//==================

StringBuilder::StringBuilder(UINT len):
m_Append(&StringBuilder::BufferAppend),
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
if(c>=0x80)
	throw InvalidArgumentException();
return (this->*m_Append)((WCHAR)c);
}

UINT StringBuilder::Append(WCHAR c)
{
return (this->*m_Append)(c);
}

UINT StringBuilder::Append(LPCSTR str)
{
if(!str)
	return 0;
UINT pos=0;
while(str[pos])
	{
	WCHAR wc=0;
	pos+=CharHelper::Read(&str[pos], &wc);
	(this->*m_Append)(wc);
	}
return pos;
}

UINT StringBuilder::Append(LPCWSTR str)
{
if(!str)
	return 0;
UINT pos=0;
while(str[pos])
	(this->*m_Append)(str[pos++]);
return pos;
}

UINT StringBuilder::Append(UINT len, LPCSTR str)
{
if(!str)
	return 0;
UINT pos=0;
while(str[pos])
	{
	if(pos>=len)
		break;
	WCHAR wc=0;
	pos+=CharHelper::Read(&str[pos], &wc);
	(this->*m_Append)(wc);
	}
return pos;
}

UINT StringBuilder::Append(UINT len, LPCWSTR str)
{
UINT pos=0;
while(str[pos])
	{
	if(pos==len)
		break;
	Append(str[pos++]);
	}
return pos;
}

VOID StringBuilder::Clear()noexcept
{
m_Append=&StringBuilder::BufferAppend;
m_Position=0;
m_Size=0;
m_String=nullptr;
m_ToString=&StringBuilder::BufferToString;
}

VOID StringBuilder::Initialize(UINT len)
{
if(len)
	{
	m_Append=&StringBuilder::StringAppend;
	m_Size=len+1;
	m_String=String::Create(len, nullptr);
	m_ToString=&StringBuilder::StringToString;
	}
else
	{
	m_Append=&StringBuilder::BufferAppend;
	m_Position=0;
	m_Size=0;
	m_String=nullptr;
	m_ToString=&StringBuilder::BufferToString;
	}
m_Position=0;
}

Handle<String> StringBuilder::ToString()
{
return (this->*m_ToString)();
}


//================
// Common Private
//================

UINT StringBuilder::BufferAppend(WCHAR c)
{
if(!m_Buffer)
	m_Buffer=StreamBuffer::Create();
#ifdef _UNICODE
UINT len=m_Buffer->Write(&c, sizeof(WCHAR));
#else
CHAR buf[4];
UINT len=CharHelper::Write(buf, 4, c);
m_Buffer->Write(buf, len);
#endif
m_Position+=len;
return len;
}

Handle<String> StringBuilder::BufferToString()
{
if(!m_Position)
	return nullptr;
auto str=String::Create(m_Position, nullptr);
auto buf=const_cast<LPTSTR>(str->Begin());
m_Buffer->Read(buf, m_Position);
buf[m_Position]=0;
str->m_Hash=StringHelper::Hash(buf);
str->m_Length=m_Position;
m_Position=0;
return str;
}

UINT StringBuilder::StringAppend(WCHAR c)
{
auto buf=const_cast<LPTSTR>(m_String->Begin());
#ifdef _UNICODE
UINT len=1;
if(m_Position+len>=m_Size)
	throw BufferOverrunException();
buf[m_Position]=c;
#else
UINT len=CharHelper::Write(&buf[m_Position], m_Size-m_Position, c);
#endif
m_Position+=len;
return len;
}

Handle<String> StringBuilder::StringToString()
{
if(m_Position+1!=m_Size)
	throw InvalidArgumentException();
auto str=m_String;
auto buf=const_cast<LPTSTR>(str->Begin());
buf[m_Position]=0;
str->m_Hash=StringHelper::Hash(buf);
str->m_Length=m_Position;
Clear();
return str;
}
