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
m_First(nullptr),
m_Last(nullptr),
m_Position(0),
m_Size(0),
m_ToString(&StringBuilder::BufferToString)
{
if(len)
	Initialize(len);
}

StringBuilder::~StringBuilder()
{
auto block=m_First;
while(block)
	{
	auto next=block->Next;
	delete block;
	block=next;
	}
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
m_Position=0;
if(len==0)
	{
	m_AppendAnsi=&StringBuilder::BufferAppendAnsi;
	m_AppendUnicode=&StringBuilder::BufferAppendUnicode;
	m_Size=0;
	m_String=nullptr;
	m_ToString=&StringBuilder::BufferToString;
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

UINT StringBuilder::BufferAppend(TCHAR tc)
{
UINT pos=m_Position%STRING_BLOCK;
if(pos==0)
	{
	if(m_Position==0)
		{
		if(m_First==nullptr)
			m_First=new StringBlock();
		m_Last=m_First;
		}
	else
		{
		auto block=m_Last->Next;
		if(!block)
			{
			block=new StringBlock();
			m_Last->Next=block;
			}
		m_Last=block;
		}
	}
m_Last->Buffer[pos]=tc;
m_Position++;
return 1;
}

UINT StringBuilder::BufferAppendAnsi(CHAR c)
{
TCHAR tc=CharHelper::ToChar(c);
return BufferAppend(tc);
}

UINT StringBuilder::BufferAppendUnicode(WCHAR c)
{
TCHAR tc=CharHelper::ToChar(c);
return BufferAppend(tc);
}

Handle<String> StringBuilder::BufferToString()
{
if(m_Position==0)
	return nullptr;
auto str=String::Create(m_Position, nullptr);
auto buf=const_cast<LPTSTR>(str->Begin());
auto block=m_First;
for(UINT pos=0; pos<m_Position; pos++)
	{
	UINT block_pos=pos%STRING_BLOCK;
	if(block_pos==0&&pos>0)
		block=block->Next;
	buf[pos]=block->Buffer[block_pos];
	}
buf[m_Position]=0;
str->m_Hash=StringHelper::GetHash(buf);
str->m_Length=m_Position;
Initialize(0);
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
