//================
// CharHelper.cpp
//================

#include "CharHelper.h"


//=======
// Using
//=======

using namespace Storage::Streams;


//============
// Conversion
//============

const CHAR CHAR_UNKNOWN=0x1A;


//============
// Comparison
//============

const CHAR STR_BREAK[]			="\n\r\t;|/\\";
const CHAR STR_LINE_BREAK[]		="\n\r";
const CHAR STR_SPECIAL[]		="\"*/:<>?\\|";

const BYTE COMPARE_CS[]=
	{
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0x00
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0x10
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0x20
//    0    1    2    3    4    5    6    7    8    9
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255, 255, 255, 255, 255, // 0x30
//         A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
	255,  10,  26,  28,  32,  36,  46,  48,  50,  52,  62,  64,  66,  68,  70,  74, // 0x40
//    P    Q    R    S    T    U    V    W    X    Y    Z
	 86,  88,  90,  92,  95,  97, 107, 109, 111, 113, 117, 255, 255, 255, 255, 255, // 0x50
//         a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
	255,  11,  27,  29,  33,  37,  47,  49,  51,  53,  63,  65,  67,  69,  71,  75, // 0x60
//    p    q    r    s    t    u    v    w    x    y    z
	 87,  89,  91,  93,  96,  98, 108, 110, 112, 114, 118, 255, 255, 255, 255, 255, // 0x70
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0x80
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0x90
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0xA0
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0xB0
//    À    Á    Â    Ã    Ä    Å    Æ    Ç    È    É    Ê    Ë    Ì    Í    Î    Ï
	 12,  14,  16,  18,  20,  22,  24,  30,  38,  40,  42,  44,  54,  56,  58,  60, // 0xC0
//    Ð    Ñ    Ò    Ó    Ô    Õ    Ö    ×    Ø    Ù    Ú    Û    Ü    Ý    Þ    ß
	 34,  72,  76,  78,  80,  82,  84, 255, 255,  99, 101, 103, 105, 115, 255,  94, // 0xD0
//    à    á    â    ã    ä    å    æ    ç    è    é    ê    ë    ì    í    î    ï
	 13,  15,  17,  19,  21,  23,  25,  31,  39,  41,  43,  45,  55,  57,  59,  61, // 0xE0
//    ð    ñ    ò    ó    ô    õ    ö    ÷    ø    ù    ú    û    ü    ý    þ    ÿ
	 35,  73,  77,  79,  81,  83,  85, 255, 255, 100, 102, 104, 106, 116, 255, 255, // 0xF0
	};

const BYTE COMPARE_NCS[]=
	{
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0x00
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0x10
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0x20
//    0    1    2    3    4    5    6    7    8    9
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255, 255, 255, 255, 255, // 0x30
//         A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
	255,  10,  18,  19,  21,  23,  28,  29,  30,  31,  36,  37,  38,  39,  40,  42, // 0x40
//    P    Q    R    S    T    U    V    W    X    Y    Z
	 48,  49,  50,  51,  53,  54,  59,  60,  61,  62,  64, 255, 255, 255, 255, 255, // 0x50
//         a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
	255,  10,  18,  19,  21,  23,  28,  29,  30,  31,  36,  37,  38,  39,  40,  42, // 0x60
//    p    q    r    s    t    u    v    w    x    y    z
	 48,  49,  50,  51,  53,  54,  59,  60,  61,  62,  64, 255, 255, 255, 255, 255, // 0x70
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0x80
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0x90
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0xA0
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0xB0
//    À    Á    Â    Ã    Ä    Å    Æ    Ç    È    É    Ê    Ë    Ì    Í    Î    Ï
	 11,  12,  13,  14,  15,  16,  17,  20,  24,  25,  26,  27,  32,  33,  34,  35, // 0xC0
//    Ð    Ñ    Ò    Ó    Ô    Õ    Ö    ×    Ø    Ù    Ú    Û    Ü    Ý    Þ    ß
	 22,  41,  43,  44,  45,  46,  47, 255, 255,  55,  56,  57,  58,  63, 255,  52, // 0xD0
//    à    á    â    ã    ä    å    æ    ç    è    é    ê    ë    ì    í    î    ï
	 11,  12,  13,  14,  15,  16,  17,  20,  24,  25,  26,  27,  32,  33,  34,  35, // 0xE0
//    ð    ñ    ò    ó    ô    õ    ö    ÷    ø    ù    ú    û    ü    ý    þ    ÿ
	 22,  41,  43,  44,  45,  46,  47, 255, 255,  55,  56,  57,  58,  63, 255, 255, // 0xF0
	};


//=============
// Char-Helper
//=============

INT CharHelper::Compare(CHAR c1, CHAR c2, BOOL cs)noexcept
{
BYTE const* sort=cs? COMPARE_CS: COMPARE_NCS;
BYTE id1=sort[(SIZE_T)c1];
BYTE id2=sort[(SIZE_T)c2];
if(id1>id2)
	return 1;
if(id1<id2)
	return -1;
if(id1!=255)
	return 0;
if(c1>c2)
	return 1;
if(c1<c2)
	return -1;
return 0;
}

INT CharHelper::Compare(CHAR c1, WCHAR wc2, BOOL cs)noexcept
{
CHAR c2=ToAnsi(wc2);
return Compare(c1, c2, cs);
}

INT CharHelper::Compare(WCHAR wc1, CHAR c2, BOOL cs)noexcept
{
CHAR c1=ToAnsi(wc1);
return Compare(c1, c2, cs);
}

INT CharHelper::Compare(WCHAR wc1, WCHAR wc2, BOOL cs)noexcept
{
CHAR c1=ToAnsi(wc1);
CHAR c2=ToAnsi(wc2);
return Compare(c1, c2, cs);
}

BOOL CharHelper::Equal(CHAR c1, CHAR c2, BOOL cs)noexcept
{
WCHAR wc1=ToUnicode(c1);
WCHAR wc2=ToUnicode(c2);
return Equal(wc1, wc2, cs);
}

BOOL CharHelper::Equal(CHAR c1, WCHAR wc2, BOOL cs)noexcept
{
WCHAR wc1=ToUnicode(c1);
return Equal(wc1, wc2, cs);
}

BOOL CharHelper::Equal(WCHAR wc1, CHAR c2, BOOL cs)noexcept
{
WCHAR wc2=ToUnicode(c2);
return Equal(wc1, wc2, cs);
}

BOOL CharHelper::Equal(WCHAR c1, WCHAR c2, BOOL cs)noexcept
{
if(!cs)
	{
	c1=ToCapitalUnicode(c1);
	c2=ToCapitalUnicode(c2);
	}
return c1==c2;
}

BOOL CharHelper::Equal(CHAR c, LPCSTR chars, BOOL cs)noexcept
{
if(!chars)
	return false;
for(UINT u=0; chars[u]; u++)
	{
	if(Equal(c, chars[u], cs))
		return true;
	}
return false;
}

BOOL CharHelper::Equal(WCHAR c, LPCSTR chars, BOOL cs)noexcept
{
if(!chars)
	return false;
for(UINT u=0; chars[u]; u++)
	{
	if(Equal(c, chars[u], cs))
		return true;
	}
return false;
}

BOOL CharHelper::Equal(WCHAR c, LPCWSTR chars, BOOL cs)noexcept
{
if(!chars)
	return false;
for(UINT u=0; chars[u]; u++)
	{
	if(Equal(c, chars[u], cs))
		return true;
	}
return false;
}

BOOL CharHelper::IsAlpha(CHAR c)noexcept
{
if(c>='A'&&c<='Z')
	return true;
if(c>='a'&&c<='z')
	return true;
return false;
}

BOOL CharHelper::IsAlpha(WCHAR c)noexcept
{
if(c>='A'&&c<='Z')
	return true;
if(c>='a'&&c<='z')
	return true;
switch(c)
	{
	case 'Ä':
	case 'ä':
	case 'Ö':
	case 'ö':
	case 'ß':
	case 'Ü':
	case 'ü':
		return true;
	}
return false;
}

BOOL CharHelper::IsBreak(CHAR c)noexcept
{
if(c==0)
	return true;
for(UINT u=0; u<TypeHelper::ArraySize(STR_BREAK); u++)
	{
	if(c==STR_BREAK[u])
		return true;
	}
return false;
}

BOOL CharHelper::IsBreak(WCHAR c)noexcept
{
if(c==0)
	return true;
for(UINT u=0; u<TypeHelper::ArraySize(STR_BREAK); u++)
	{
	if(c==STR_BREAK[u])
		return true;
	}
return false;
}

BOOL CharHelper::IsCapital(CHAR c)noexcept
{
if(c>='A'&&c<='Z')
	return true;
return false;
}

BOOL CharHelper::IsCapital(WCHAR c)noexcept
{
if(c>='A'&&c<='Z')
	return true;
switch(c)
	{
	case 'Ä':
	case 'Ö':
	case 'Ü':
		return true;
	}
return false;
}

BOOL CharHelper::IsLineBreak(CHAR c)noexcept
{
if(c==0)
	return true;
for(UINT u=0; u<TypeHelper::ArraySize(STR_LINE_BREAK); u++)
	{
	if(c==STR_LINE_BREAK[u])
		return true;
	}
return false;
}

BOOL CharHelper::IsLineBreak(WCHAR c)noexcept
{
if(c==0)
	return true;
for(UINT u=0; u<TypeHelper::ArraySize(STR_LINE_BREAK); u++)
	{
	if(c==STR_LINE_BREAK[u])
		return true;
	}
return false;
}

BOOL CharHelper::IsPrintable(CHAR c)noexcept
{
if(c<0x20)
	return false;
if(c>0x7E)
	return false;
return true;
}

BOOL CharHelper::IsPrintable(WCHAR c)noexcept
{
if(c<0x20)
	return false;
if(c>0x7E&&c<0xA0)
	return false;
return true;
}

BOOL CharHelper::IsSmall(CHAR c)noexcept
{
if(c>='a'&&c<='z')
	return true;
return false;
}

BOOL CharHelper::IsSmall(WCHAR c)noexcept
{
if(c>='a'&&c<='z')
	return true;
switch(c)
	{
	case 'ä':
	case 'ö':
	case 'ß':
	case 'ü':
		return true;
	}
return false;
}

BOOL CharHelper::IsSpecial(CHAR c)noexcept
{
if(c==0)
	return false;
for(UINT u=0; u<TypeHelper::ArraySize(STR_SPECIAL); u++)
	{
	if(c==STR_SPECIAL[u])
		return true;
	}
return false;
}

BOOL CharHelper::IsSpecial(WCHAR c)noexcept
{
if(c==0)
	return false;
for(UINT u=0; u<TypeHelper::ArraySize(STR_SPECIAL); u++)
	{
	if(c==STR_SPECIAL[u])
		return true;
	}
return false;
}

UINT CharHelper::Read(LPCSTR str)
{
return Read(str, (WCHAR*)nullptr);
}

UINT CharHelper::Read(LPCSTR str, CHAR* c_ptr)
{
if(!c_ptr)
	return Read(str);
WCHAR c=0;
UINT len=Read(str, &c);
*c_ptr=ToAnsi(c);
return len;
}

UINT CharHelper::Read(LPCSTR str, WCHAR* c_ptr)
{
if(!str)
	return 0;
auto buf=(BYTE const*)str;
if((buf[0]&0xF0)==0xF0)
	{
	if(c_ptr)
		*c_ptr=CHAR_UNKNOWN;
	return 4;
	}
if((buf[0]&0xE0)==0xE0)
	{
	if(c_ptr)
		{
		WCHAR c=0;
		c|=((WCHAR)buf[0]&0x0F)<<12;
		c|=((WCHAR)buf[1]&0x3F)<<6;
		c|=((WCHAR)buf[2]&0x3F);
		*c_ptr=c;
		}
	return 3;
	}
if((buf[0]&0xC0)==0xC0)
	{
	if(c_ptr)
		{
		WCHAR c=0;
		c|=((WCHAR)buf[0]&0x1F)<<6;
		c|=((WCHAR)buf[1]&0x3F);
		*c_ptr=c;
		}
	return 2;
	}
if(c_ptr)
	{
	CHAR c=(CHAR)(buf[0]&0x7F);
	*c_ptr=c;
	}
return 1;
}

UINT CharHelper::Read(LPCWSTR str)
{
return 1;
}

UINT CharHelper::Read(LPCWSTR str, CHAR* c_ptr)
{
if(!str)
	return 0;
if(c_ptr)
	*c_ptr=ToAnsi(str[0]);
return 1;
}

UINT CharHelper::Read(LPCWSTR str, WCHAR* c_ptr)
{
if(!str)
	return 0;
if(c_ptr)
	*c_ptr=str[0];
return 1;
}

UINT CharHelper::Read(InputStream* stream)
{
return Read(stream, (WCHAR*)nullptr);
}

UINT CharHelper::Read(InputStream* stream, CHAR* c_ptr)
{
if(!c_ptr)
	return Read(stream);
WCHAR c=0;
UINT len=Read(stream, &c);
*c_ptr=ToAnsi(c);
return len;
}

UINT CharHelper::Read(InputStream* stream, WCHAR* c_ptr)
{
if(!stream)
	return 0;
UINT read=0;
BYTE buf[4]={ 0 };
read+=stream->Read(&buf[0], 1);
if((buf[0]&0xF0)==0xF0)
	{
	read+=stream->Read(&buf[1], 3);
	if(c_ptr)
		*c_ptr=CHAR_UNKNOWN;
	return read;
	}
if((buf[0]&0xE0)==0xE0)
	{
	read+=stream->Read(&buf[1], 2);
	if(c_ptr)
		{
		WCHAR c=0;
		c|=((WCHAR)buf[0]&0x0F)<<12;
		c|=((WCHAR)buf[1]&0x3F)<<6;
		c|=((WCHAR)buf[2]&0x3F);
		*c_ptr=c;
		}
	return read;
	}
if((buf[0]&0xC0)==0xC0)
	{
	read+=stream->Read(&buf[1], 1);
	if(c_ptr)
		{
		WCHAR c=0;
		c|=((WCHAR)buf[0]&0x1F)<<6;
		c|=((WCHAR)buf[1]&0x3F);
		*c_ptr=c;
		}
	return read;
	}
if(c_ptr)
	{
	CHAR c=(CHAR)(buf[0]&0x7F);
	*c_ptr=c;
	}
return read;
}

CHAR CharHelper::ToAnsi(WCHAR wc)noexcept
{
if(wc<0x80)
	return (CHAR)wc;
return CHAR_UNKNOWN;
}

CHAR CharHelper::ToCapitalAnsi(CHAR c)noexcept
{
WCHAR wc=ToUnicode(c);
wc=ToCapitalUnicode(wc);
return ToAnsi(wc);
}

CHAR CharHelper::ToCapitalAnsi(WCHAR c)noexcept
{
c=ToCapitalUnicode(c);
return ToAnsi(c);
}

WCHAR CharHelper::ToCapitalUnicode(CHAR c)noexcept
{
WCHAR wc=ToUnicode(c);
return ToCapitalUnicode(wc);
}

WCHAR CharHelper::ToCapitalUnicode(WCHAR c)noexcept
{
if(c>='a'&&c<='z')
	return c-0x20;
switch(c)
	{
	case 'ä':
		return 'Ä';
	case 'ö':
		return 'Ö';
	case 'ü':
		return 'Ü';
	}
return c;
}

BOOL CharHelper::ToDigit(CHAR c, UINT* digit_ptr, UINT base)noexcept
{
if(c<'0')
	return false;
if(c>='a')
	{
	c-='a';
	}
else if(c>='A')
	{
	c-='A';
	}
c-='0';
UINT digit=(UINT)c;
if(digit<base)
	{
	if(digit_ptr)
		*digit_ptr=digit;
	return true;
	}
return false;
}

BOOL CharHelper::ToDigit(WCHAR wc, UINT* digit_ptr, UINT base)noexcept
{
CHAR c=ToAnsi(wc);
return ToDigit(c, digit_ptr, base);
}

CHAR CharHelper::ToSmallAnsi(CHAR c)noexcept
{
WCHAR wc=ToUnicode(c);
wc=ToSmallUnicode(wc);
return ToAnsi(wc);
}

CHAR CharHelper::ToSmallAnsi(WCHAR c)noexcept
{
c=ToSmallUnicode(c);
return ToAnsi(c);
}

WCHAR CharHelper::ToSmallUnicode(CHAR c)noexcept
{
WCHAR wc=ToUnicode(c);
return ToSmallUnicode(wc);
}

WCHAR CharHelper::ToSmallUnicode(WCHAR c)noexcept
{
if(c>='A'&&c<='Z')
	return c+0x20;
switch(c)
	{
	case 'Ä':
		return 'ä';
	case 'Ö':
		return 'ö';
	case 'Ü':
		return 'ü';
	}
return c;
}

WCHAR CharHelper::ToUnicode(CHAR c)noexcept
{
BYTE b=(BYTE)c;
if(b<0x80)
	return (WCHAR)c;
return CHAR_UNKNOWN;
}

UINT CharHelper::Write(LPSTR str, UINT size, CHAR c)
{
if(size<1)
	throw BufferOverrunException();
str[0]=c;
return 1;
}

UINT CharHelper::Write(LPSTR str, UINT size, WCHAR c)
{
if(c<0x80)
	{
	if(!str)
		return 1;
	if(size<1)
		throw BufferOverrunException();
	str[0]=(CHAR)c;
	return 1;
	}
if(c<0x800)
	{
	if(!str)
		return 2;
	if(size<2)
		throw BufferOverrunException();
	str[0]=(CHAR)(((c>>6)&0x1F)|0xC0);
	str[1]=(CHAR)((c&0x3F)|0x80);
	return 2;
	}
if(!str)
	return 3;
if(size<3)
	throw BufferOverrunException();
str[0]=(CHAR)(((c>>12)&0xF)|0xE0);
str[1]=(CHAR)(((c>>6)&0x3F)|0x80);
str[2]=(CHAR)((c&0x3F)|0x80);
return 3;
}

UINT CharHelper::Write(LPWSTR str, UINT size, CHAR c)
{
if(!str)
	return 1;
if(size<1)
	throw BufferOverrunException();
str[0]=ToUnicode(c);
return 1;
}

UINT CharHelper::Write(LPWSTR str, UINT size, WCHAR c)
{
if(!str)
	return 1;
if(size<1)
	throw BufferOverrunException();
str[0]=c;
return 1;
}

UINT CharHelper::Write(OutputStream* stream, CHAR c)
{
if(!stream)
	return 1;
return (UINT)stream->Write(&c, 1);
}

UINT CharHelper::Write(OutputStream* stream, WCHAR c)
{
if(!stream)
	return Write((LPSTR)nullptr, 0, c);
CHAR str[4];
UINT len=Write(str, 4, c);
return (UINT)stream->Write(str, len);
}
