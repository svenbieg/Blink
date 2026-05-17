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

INT CharHelper::Compare(CHAR c1, CHAR c2)noexcept
{
BYTE b1=TypeHelper::Min((BYTE)0x7F, (BYTE)c1);
BYTE b2=TypeHelper::Min((BYTE)0x7F, (BYTE)c2);
BYTE id1=COMPARE_CS[b1];
BYTE id2=COMPARE_CS[b2];
if(id1!=0xFF&&id2!=0xFF)
	{
	b1=id1;
	b2=id2;
	}
if(b1>b2)
	return 1;
if(b1<b2)
	return -1;
return 0;
}

INT CharHelper::Compare(CHAR c1, CHAR c2, CompareMode mode)noexcept
{
BYTE b1=TypeHelper::Min((BYTE)0x7F, (BYTE)c1);
BYTE b2=TypeHelper::Min((BYTE)0x7F, (BYTE)c2);
BYTE id1=COMPARE_NCS[b1];
BYTE id2=COMPARE_NCS[b2];
if(id1!=0xFF&&id2!=0xFF)
	{
	b1=id1;
	b2=id2;
	}
if(b1>b2)
	return 1;
if(b1<b2)
	return -1;
return 0;
}

INT CharHelper::Compare(CHAR c1, WCHAR wc2)noexcept
{
CHAR c2=ToChar(wc2);
return Compare(c1, c2);
}

INT CharHelper::Compare(CHAR c1, WCHAR wc2, CompareMode mode)noexcept
{
CHAR c2=ToChar(wc2);
return Compare(c1, c2, mode);
}

INT CharHelper::Compare(WCHAR wc1, CHAR c2)noexcept
{
CHAR c1=ToChar(wc1);
return Compare(c1, c2);
}

INT CharHelper::Compare(WCHAR wc1, CHAR c2, CompareMode mode)noexcept
{
CHAR c1=ToChar(wc1);
return Compare(c1, c2, mode);
}

INT CharHelper::Compare(WCHAR c1, WCHAR c2)noexcept
{
WORD w1=TypeHelper::Min((WORD)0xFF, (WORD)c1);
WORD w2=TypeHelper::Min((WORD)0xFF, (WORD)c2);
BYTE id1=COMPARE_CS[w1];
BYTE id2=COMPARE_CS[w2];
if(id1!=0xFF&&id2!=0xFF)
	{
	w1=id1;
	w2=id2;
	}
if(w1>w2)
	return 1;
if(w1<w2)
	return -1;
return 0;
}

INT CharHelper::Compare(WCHAR c1, WCHAR c2, CompareMode mode)noexcept
{
WORD w1=TypeHelper::Min((WORD)0xFF, (WORD)c1);
WORD w2=TypeHelper::Min((WORD)0xFF, (WORD)c2);
BYTE id1=COMPARE_NCS[w1];
BYTE id2=COMPARE_NCS[w2];
if(id1!=0xFF&&id2!=0xFF)
	{
	w1=id1;
	w2=id2;
	}
if(w1>w2)
	return 1;
if(w1<w2)
	return -1;
return 0;
}

BOOL CharHelper::Equal(CHAR c1, CHAR c2, CompareMode mode)noexcept
{
if(c1==c2)
	return true;
c1=ToCapital(c1);
c2=ToCapital(c2);
return c1==c2;
}

BOOL CharHelper::Equal(CHAR c1, WCHAR wc2)noexcept
{
CHAR c2=ToChar(wc2);
return Equal(c1, c2);
}

BOOL CharHelper::Equal(CHAR c1, WCHAR wc2, CompareMode mode)noexcept
{
CHAR c2=ToChar(wc2);
return Equal(c1, c2, mode);
}

BOOL CharHelper::Equal(WCHAR wc1, CHAR c2)noexcept
{
CHAR c1=ToChar(wc1);
return Equal(c1, c2);
}

BOOL CharHelper::Equal(WCHAR wc1, CHAR c2, CompareMode mode)noexcept
{
CHAR c1=ToChar(wc1);
return Equal(c1, c2, mode);
}

BOOL CharHelper::Equal(WCHAR c1, WCHAR c2, CompareMode mode)noexcept
{
if(c1==c2)
	return true;
c1=ToCapitalW(c1);
c2=ToCapitalW(c2);
return c1==c2;
}

BOOL CharHelper::Equal(CHAR c, LPCSTR chars)noexcept
{
if(!chars)
	return false;
UINT pos=0;
while(chars[pos])
	{
	if(Equal(c, chars[pos++]))
		return true;
	}
return false;
}

BOOL CharHelper::Equal(CHAR c, LPCSTR chars, CompareMode mode)noexcept
{
if(!chars)
	return false;
UINT pos=0;
while(chars[pos])
	{
	if(Equal(c, chars[pos++], mode))
		return true;
	}
return false;
}

BOOL CharHelper::Equal(WCHAR c, LPCSTR chars)noexcept
{
if(!chars)
	return false;
UINT pos=0;
while(chars[pos])
	{
	WCHAR wc=0;
	pos+=Read(&chars[pos], &wc);
	if(Equal(c, wc))
		return true;
	}
return false;
}

BOOL CharHelper::Equal(WCHAR c, LPCSTR chars, CompareMode mode)noexcept
{
if(!chars)
	return false;
UINT pos=0;
while(chars[pos])
	{
	WCHAR wc=0;
	pos+=Read(&chars[pos], &wc);
	if(Equal(c, wc, mode))
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
	case L'Ä':
	case L'ä':
	case L'Ö':
	case L'ö':
	case L'ß':
	case L'Ü':
	case L'ü':
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
	case L'Ä':
	case L'Ö':
	case L'Ü':
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
	case L'ä':
	case L'ö':
	case L'ß':
	case L'ü':
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

UINT CharHelper::Read(LPCSTR str, CHAR* c_ptr)
{
if(!c_ptr)
	return Read(str);
WCHAR c=0;
UINT len=Read(str, &c);
*c_ptr=ToChar(c);
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

UINT CharHelper::Read(LPCWSTR str, CHAR* c_ptr)
{
if(!str)
	return 0;
if(c_ptr)
	*c_ptr=ToChar(str[0]);
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

UINT CharHelper::Read(InputStream* stream, CHAR* c_ptr)
{
if(!c_ptr)
	return Read(stream);
WCHAR c=0;
UINT len=Read(stream, &c);
*c_ptr=ToChar(c);
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

CHAR CharHelper::ToCapital(CHAR c)noexcept
{
WCHAR wc=ToCharW(c);
wc=ToCapitalW(wc);
return ToChar(wc);
}

CHAR CharHelper::ToCapital(WCHAR c)noexcept
{
c=ToCapitalW(c);
return ToChar(c);
}

WCHAR CharHelper::ToCapitalW(CHAR c)noexcept
{
WCHAR wc=ToCharW(c);
return ToCapitalW(wc);
}

WCHAR CharHelper::ToCapitalW(WCHAR c)noexcept
{
if(c>='a'&&c<='z')
	return c-0x20;
switch(c)
	{
	case L'ä':
		return L'Ä';
	case L'ö':
		return L'Ö';
	case L'ü':
		return L'Ü';
	}
return c;
}

CHAR CharHelper::ToChar(WCHAR wc)noexcept
{
if(wc<0x80)
	return (CHAR)wc;
return CHAR_UNKNOWN;
}

WCHAR CharHelper::ToCharW(CHAR c)noexcept
{
BYTE b=(BYTE)c;
if(b<0x80)
	return (WCHAR)c;
return CHAR_UNKNOWN;
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
CHAR c=ToChar(wc);
return ToDigit(c, digit_ptr, base);
}

CHAR CharHelper::ToSmall(CHAR c)noexcept
{
WCHAR wc=ToCharW(c);
wc=ToSmallW(wc);
return ToChar(wc);
}

CHAR CharHelper::ToSmall(WCHAR c)noexcept
{
c=ToSmallW(c);
return ToChar(c);
}

WCHAR CharHelper::ToSmallW(CHAR c)noexcept
{
WCHAR wc=ToCharW(c);
return ToSmallW(wc);
}

WCHAR CharHelper::ToSmallW(WCHAR c)noexcept
{
if(c>='A'&&c<='Z')
	return c+0x20;
switch(c)
	{
	case L'Ä':
		return L'ä';
	case L'Ö':
		return L'ö';
	case L'Ü':
		return L'ü';
	}
return c;
}

UINT CharHelper::Write(LPSTR str, UINT size, CHAR c)
{
if(!str)
	return 1;
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
str[0]=ToCharW(c);
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
