//=================
// StringClass.cpp
//=================

#include "pch.h"


//======
// Hash
//======

constexpr UINT BITS_PER_CHAR=5;
constexpr UINT CHARS_PER_HASH=6;

constexpr BYTE HASH_CODE[]=
{
//     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x00
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x10
//           !     "     #     $     %     &     '     (     )     *     +     ,     -     .     /
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 0x20
//     0     1     2     3     4     5     6     7     8     9     :     ;     <     =     >     ?
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 0x30
//           A     B     C     D     E     F     G     H     I     J     K     L     M     N     O
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, // 0x40
//     P     Q     R     S     T     U     V     W     X     Y     Z     [     \     ]     ^     _
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x01, 0x01, 0x01, 0x01, 0x01, // 0x50
//     `     a     b     c     d     e     f     g     h     i     j     k     l     m     n     o
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, // 0x60
//     p     q     r     s     t     u     v     w     x     y     z     {     |     }     ~
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x01, 0x01, 0x01, 0x01, 0x00, // 0x70
//     €
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x80
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x90
//                                               §
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xA0
//                 ²     ³           µ
	0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xB0
//     À     Á     Â     Ã     Ä     Å     Æ     Ç     È     É     Ê     Ë     Ì     Í     Î     Ï
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x06, 0x06, 0x06, 0x06, 0x0A, 0x0A, 0x0A, 0x0A, // 0xC0
//     Ð     Ñ     Ò     Ó     Ô     Õ     Ö     ×     Ø     Ù     Ú     Û     Ü     Ý     Þ     ß
	0x05, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x01, 0x01, 0x16, 0x16, 0x16, 0x16, 0x1A, 0x01, 0x14, // 0xD0
//     à     á     â     ã     ä     å     æ     ç     è     é     ê     ë     ì     í     î     ï
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x06, 0x06, 0x06, 0x06, 0x0A, 0x0A, 0x0A, 0x0A, // 0xE0
//     ð     ñ     ò     ó     ô     õ     ö     ÷     ø     ù     ú     û     ü     ý     þ     ÿ
	0x05, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x01, 0x01, 0x16, 0x16, 0x16, 0x16, 0x1A, 0x01, 0x1A, // 0xF0
};


//==================
// Con-/Destructors
//==================

Handle<String> String::Create(LPCSTR Value)
{
UINT len=StringHelper::Length(Value);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(LPCWSTR Value)
{
UINT len=StringHelper::Length(Value);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(UINT Length, nullptr_t)
{
UINT size=sizeof(String)+(Length+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf);
return str;
}

Handle<String> String::Create(UINT Length, LPCSTR Value)
{
UINT len=StringHelper::Length(Value, Length);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(UINT Length, LPCWSTR Value)
{
UINT len=StringHelper::Length(Value, Length);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(LPCSTR Format, VariableArguments const& Arguments)
{
UINT len=StringHelper::Length(Format, Arguments);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Format, Arguments);
return str;
}


//========
// Access
//========

UINT String::GetLength()
{
if(!m_Length)
	m_Length=StringHelper::Length(m_Buffer);
return m_Length;
}

Handle<String> String::ToString(LanguageCode lng)
{
return this;
}


//============
// Comparison
//============

INT String::Compare(String* str1, String* str2)
{
UINT64 hash1=Hash(str1);
UINT64 hash2=Hash(str2);
if(hash1<hash2)
	return -1;
if(hash1>hash2)
	return 1;
if(!hash1)
	return 0;
auto buf1=str1? str1->Begin(): nullptr;
auto buf2=str2? str2->Begin(): nullptr;
return StringHelper::Compare(buf1, buf2);
}


//===========
// Operators
//===========

Handle<String> String::Replace(LPCSTR find, LPCSTR replace, BOOL cs, BOOL repeat)
{
UINT len=StringHelper::Replace(nullptr, 0, m_Buffer, find, replace);
if(!len)
	return nullptr;
auto str=String::Create(len+1, nullptr);
LPTSTR buf=const_cast<LPTSTR>(str->Begin());
StringHelper::Replace(buf, len+1, m_Buffer, find, replace, cs, repeat);
return str;
}


//================
// Common Private
//================

UINT64 String::Hash(String* str)
{
if(!str)
	return 0;
if(str->m_Hash!=INVALID_HASH)
	return str->m_Hash;
UINT64 hash=0;
auto buf=str->Begin();
UINT pos=0;
for(; pos<CHARS_PER_HASH; pos++)
	{
	auto c=CharHelper::ToAnsi(buf[pos]);
	if(!c)
		break;
	auto code=HASH_CODE[(BYTE)c];
	hash<<=BITS_PER_CHAR;
	hash|=code;
	}
hash<<=((CHARS_PER_HASH-pos)*BITS_PER_CHAR);
str->m_Hash=hash;
return hash;
}
