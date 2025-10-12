//================
// CharHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "CharHelper.h"

using namespace Storage::Streams;


//============
// Conversion
//============

WCHAR UnicodeMap[128]=
{
//       0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F
//	     €               ‚       ƒ       „       …       †       ‡       ˆ       ‰       Š       ‹       Œ               Ž        
	0x20AC, 0x005F, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x005F, 0x017D, 0x005F, // 0x80
//	             ‘       ’       “       ”       •       –       —       ˜       ™       š       ›       œ       ť       ž       Ÿ
	0x005F, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x0165, 0x017E, 0x0178, // 0x90
//               ¡       ¢       £       ¤       ¥       ¦       §       ¨       ©       ª       «       ¬               ­®       ¯
	0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7, 0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x005F, 0x00AE, 0x00AF, // 0xA0
//	     °       ­­­­±       ²       ³       ´       µ       ¶       ·       ¸       ¹       º       »       ¼       ½       ¾       ¿
	0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF, // 0xB0
//	     À       Á       Â       Ã       Ä       Å       Æ       Ç       È       É       Ê       Ë       Ì       Í       Î       Ï
	0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, // 0xC0
//	     Ð       Ñ       Ò       Ó       Ô       Õ       Ö       ×       Ø       Ù       Ú       Û       Ü       Ý       Þ       ß
	0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF, // 0xD0
//	     à       á       â       ã       ä       å       æ       ç       è       é       ê       ë       ì       í       î       ï
	0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7, 0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, // 0xE0
//       ð       ñ       ò       ó       ô       õ       ö       ÷       ø       ù       ú       û       ü       ý       þ       ÿ
	0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF, // 0xF0
};

template <class _dst_t, class _src_t> inline _dst_t CharToChar(_src_t c)
{
return c;
}

template <> inline CHAR CharToChar(WCHAR c)
{
return CharHelper::ToAnsi(c);
}

template <> inline WCHAR CharToChar(CHAR c)
{
return CharHelper::ToUnicode(c);
}

template <class _dst_t, class _src_t> inline _dst_t CharToCapital(_src_t tc)
{
CHAR c=CharToChar<CHAR, _src_t>(tc);
if(c>='a'&&c<='z')
	return CharToChar<_dst_t, CHAR>((CHAR)(c-0x20));
switch(c)
	{
	case Ansi::ae:
		return CharToChar<_dst_t, CHAR>(Ansi::AE);
	case Ansi::oe:
		return CharToChar<_dst_t, CHAR>(Ansi::OE);
	case Ansi::ue:
		return CharToChar<_dst_t, CHAR>(Ansi::UE);
	}
return CharToChar<_dst_t, _src_t>(tc);
}

template <class _char_t> inline BOOL CharToDigit(_char_t tc, UINT* digit_ptr, UINT base)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
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

template <class _dst_t, class _src_t> inline _dst_t CharToSmall(_src_t tc)
{
CHAR c=CharToChar<_dst_t, _src_t>(tc);
if(c>='A'&&c<='Z')
	return CharToChar<_dst_t, CHAR>((CHAR)(c+0x20));
switch(c)
	{
	case Ansi::AE:
		return CharToChar<_dst_t, CHAR>(Ansi::ae);
	case Ansi::OE:
		return CharToChar<_dst_t, CHAR>(Ansi::oe);
	case Ansi::UE:
		return CharToChar<_dst_t, CHAR>(Ansi::ue);
	}
return CharToChar<_dst_t, _src_t>(tc);
}


//=======
// UTF-8
//=======

template <class _char_t> UINT CharReadUtf8(InputStream* stream, _char_t* c_ptr)
{
if(!stream||!c_ptr)
	return 0;
UINT size=0;
BYTE buf[4];
UINT read=(UINT)stream->Read(&buf[0], 1);
if(read==1)
	{
	size+=read;
	if((buf[0]&0xF0)==0xF0)
		{
		read=(UINT)stream->Read(&buf[1], 3);
		size+=read;
		if(read==3)
			{
			*c_ptr=CharToChar<_char_t, CHAR>('_');
			return size;
			}
		}
	else if((buf[0]&0xE0)==0xE0)
		{
		read=(UINT)stream->Read(&buf[1], 2);
		size+=read;
		if(read==2)
			{
			*c_ptr=CharToChar<_char_t, CHAR>('_');
			return size;
			}
		}
	else if((buf[0]&0xC0)==0xC0)
		{
		read=(UINT)stream->Read(&buf[1], 1);
		size+=read;
		if(read==1)
			{
			WCHAR c=0;
			c|=((buf[0]&0x1F)<<8);
			c|=((buf[1]&0x3F));
			*c_ptr=CharToChar<_char_t, WCHAR>(c);
			return size;
			}
		}
	else
		{
		CHAR c=(CHAR)(buf[0]&0x7F);
		*c_ptr=CharToChar<_char_t, CHAR>(c);;
		return 1;
		}
	}
*c_ptr=0;
return size;
}

template <class _char_t> UINT CharWriteUtf8(OutputStream* stream, _char_t tc)
{
WCHAR c=CharToChar<WCHAR, _char_t>(tc);
if(c<0x80)
	{
	if(!stream)
		return 1;
	BYTE buf=(BYTE)c;
	return (UINT)stream->Write(&buf, 1);
	}
if(c<0x800)
	{
	if(!stream)
		return 2;
	BYTE buf[2];
	buf[0]=((c>>6)&0x1F)|0xC0;
	buf[1]=(c&0x3F)|0x80;
	return (UINT)stream->Write(buf, 2);
	}
if(!stream)
	return 3;
BYTE buf[3];
buf[0]=((c>>12)&0xF)|0xE0;
buf[1]=((c>>6)&0x3F)|0x80;
buf[2]=(c&0x3F)|0x80;
return (UINT)stream->Write(buf, 3);
}


//============
// Comparison
//============

const BYTE CharCompareCaseSensitive[]=
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

const BYTE CharCompareNotCaseSensitive[]=
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

template <class _char1_t, class _char2_t> inline INT CharCompare(_char1_t tc1, _char2_t tc2, BOOL cs)
{
CHAR c1=CharToChar<CHAR, _char1_t>(tc1);
CHAR c2=CharToChar<CHAR, _char2_t>(tc2);
BYTE const* sort=cs? CharCompareCaseSensitive: CharCompareNotCaseSensitive;
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

template <class _char1_t, class _char2_t> inline BOOL CharEqual(_char1_t tc1, _char2_t tc2, BOOL cs)
{
CHAR c1=CharToChar<CHAR, _char1_t>(tc1);
CHAR c2=CharToChar<CHAR, _char2_t>(tc2);
if(!cs)
	{
	c1=CharHelper::ToCapitalAnsi(c1);
	c2=CharHelper::ToCapitalAnsi(c2);
	}
return c1==c2;
}

template <class _char_t> inline BOOL CharIsAlpha(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c>='A'&&c<='Z')
	return true;
if(c>='a'&&c<='z')
	return true;
switch(c)
	{
	case Ansi::AE:
	case Ansi::ae:
	case Ansi::OE:
	case Ansi::oe:
	case Ansi::UE:
	case Ansi::ue:
		return true;
	}
return false;
}

template <class _char_t> inline BOOL CharIsBreak(_char_t tc)
{
if(tc==0)
	return true;
CHAR c=CharToChar<CHAR, _char_t>(tc);
CHAR str[]="\n\r\t;:&|+*/\\?!";
for(UINT u=0; u<TypeHelper::ArraySize(str); u++)
	{
	if(c==str[u])
		return true;
	}
return false;
}

template <class _char_t> inline BOOL CharIsCapital(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c>='A'&&c<='Z')
	return true;
switch(c)
	{
	case Ansi::AE:
	case Ansi::OE:
	case Ansi::UE:
		return true;
	}
return false;
}

template <class _char_t> inline BOOL CharIsPrintable(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c>=' '&&c<='~')
	return true;
return false;
}

template <class _char_t> inline BOOL CharIsSmall(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c>='a'&&c<='z')
	return true;
switch(c)
	{
	case Ansi::ae:
	case Ansi::oe:
	case Ansi::ue:
		return true;
	}
return false;
}

template <class _char_t> inline BOOL CharIsSpecial(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
CHAR str[]="\"*/:<>?\\|";
for(UINT u=0; u<TypeHelper::ArraySize(str); u++)
	{
	if(c==str[u])
		return true;
	}
return false;
}


//=============
// Char-Helper
//=============

INT CharHelper::Compare(CHAR c1, CHAR c2, BOOL cs)
{
return CharCompare(c1, c2, cs);
}

INT CharHelper::Compare(CHAR c1, WCHAR c2, BOOL cs)
{
return CharCompare(c1, c2, cs);
}

INT CharHelper::Compare(WCHAR c1, CHAR c2, BOOL cs)
{
return CharCompare(c1, c2, cs);
}

INT CharHelper::Compare(WCHAR c1, WCHAR c2, BOOL cs)
{
return CharCompare(c1, c2, cs);
}

BOOL CharHelper::Equal(CHAR c1, CHAR c2, BOOL cs)
{
return CharEqual(c1, c2, cs);
}

BOOL CharHelper::Equal(CHAR c1, WCHAR c2, BOOL cs)
{
return CharEqual(c1, c2, cs);
}

BOOL CharHelper::Equal(WCHAR c1, CHAR c2, BOOL cs)
{
return CharEqual(c1, c2, cs);
}

BOOL CharHelper::Equal(WCHAR c1, WCHAR c2, BOOL cs)
{
return CharEqual(c1, c2, cs);
}

BOOL CharHelper::Equal(CHAR c, LPCSTR chars, BOOL cs)
{
if(!chars)
	return false;
for(UINT u=0; chars[u]; u++)
	{
	if(CharEqual(c, chars[u], cs))
		return true;
	}
return false;
}

BOOL CharHelper::Equal(WCHAR c, LPCSTR chars, BOOL cs)
{
if(!chars)
	return false;
for(UINT u=0; chars[u]; u++)
	{
	if(CharEqual(c, chars[u], cs))
		return true;
	}
return false;
}

BOOL CharHelper::Equal(WCHAR c, LPCWSTR chars, BOOL cs)
{
if(!chars)
	return false;
for(UINT u=0; chars[u]; u++)
	{
	if(CharEqual(c, chars[u], cs))
		return true;
	}
return false;
}

BOOL CharHelper::IsAlpha(CHAR c)
{
return CharIsAlpha(c);
}

BOOL CharHelper::IsAlpha(WCHAR c)
{
return CharIsAlpha(c);
}

BOOL CharHelper::IsBreak(CHAR c)
{
return CharIsBreak(c);
}

BOOL CharHelper::IsBreak(WCHAR c)
{
return CharIsBreak(c);
}

BOOL CharHelper::IsCapital(CHAR c)
{
return CharIsCapital(c);
}

BOOL CharHelper::IsCapital(WCHAR c)
{
return CharIsCapital(c);
}

BOOL CharHelper::IsPrintable(CHAR c)
{
return CharIsPrintable(c);
}

BOOL CharHelper::IsPrintable(WCHAR c)
{
return CharIsPrintable(c);
}

BOOL CharHelper::IsSmall(CHAR c)
{
return CharIsSmall(c);
}

BOOL CharHelper::IsSmall(WCHAR c)
{
return CharIsSmall(c);
}

BOOL CharHelper::IsSpecial(CHAR c)
{
return CharIsSpecial(c);
}

BOOL CharHelper::IsSpecial(WCHAR c)
{
return CharIsSpecial(c);
}

UINT CharHelper::ReadAnsi(InputStream* stream, CHAR* c_ptr)
{
if(!stream)
	return 0;
return (UINT)stream->Read(c_ptr, sizeof(CHAR));
}

UINT CharHelper::ReadAnsi(InputStream* stream, WCHAR* c_ptr)
{
if(!stream)
	return 0;
CHAR c=0;
UINT read=(UINT)stream->Read(&c, sizeof(CHAR));
if(c_ptr)
	*c_ptr=CharToChar<WCHAR, CHAR>(c);
return read;
}

UINT CharHelper::ReadUnicode(InputStream* stream, CHAR* c_ptr)
{
if(!stream)
	return 0;
WCHAR c=0;
UINT read=(UINT)stream->Read(&c, sizeof(WCHAR));
if(c_ptr)
	*c_ptr=CharToChar<CHAR, WCHAR>(c);
return read;
}

UINT CharHelper::ReadUnicode(InputStream* stream, WCHAR* c_ptr)
{
if(!stream)
	return 0;
return (UINT)stream->Read(c_ptr, sizeof(WCHAR));
}

UINT CharHelper::ReadUtf8(InputStream* stream, CHAR* c_ptr)
{
if(!stream)
	return 0;
return CharReadUtf8(stream, c_ptr);
}

UINT CharHelper::ReadUtf8(InputStream* stream, WCHAR* c_ptr)
{
if(!stream)
	return 0;
return CharReadUtf8(stream, c_ptr);
}

CHAR CharHelper::ToAnsi(WCHAR wc)
{
if(wc<0x80)
	return (CHAR)wc;
for(UINT u=0; u<128; u++)
	{
	if(UnicodeMap[u]==wc)
		return (CHAR)(u+0x80);
	}
return '_';
}

TCHAR CharHelper::ToCapital(CHAR c)
{
return CharToCapital<TCHAR, CHAR>(c);
}

TCHAR CharHelper::ToCapital(WCHAR c)
{
return CharToCapital<TCHAR, WCHAR>(c);
}

CHAR CharHelper::ToCapitalAnsi(CHAR c)
{
return CharToCapital<CHAR, CHAR>(c);
}

CHAR CharHelper::ToCapitalAnsi(WCHAR c)
{
return CharToCapital<CHAR, WCHAR>(c);
}

WCHAR CharHelper::ToCapitalUnicode(CHAR c)
{
return CharToCapital<WCHAR, CHAR>(c);
}

WCHAR CharHelper::ToCapitalUnicode(WCHAR c)
{
return CharToCapital<WCHAR, WCHAR>(c);
}

TCHAR CharHelper::ToChar(CHAR c)
{
return CharToChar<TCHAR, CHAR>(c);
}

TCHAR CharHelper::ToChar(WCHAR c)
{
return CharToChar<TCHAR, WCHAR>(c);
}

BOOL CharHelper::ToDigit(CHAR c, UINT* digit_ptr, UINT base)
{
return CharToDigit(c, digit_ptr, base);
}

BOOL CharHelper::ToDigit(WCHAR c, UINT* digit_ptr, UINT base)
{
return CharToDigit(c, digit_ptr, base);
}

TCHAR CharHelper::ToSmall(CHAR c)
{
return CharToSmall<TCHAR, CHAR>(c);
}

TCHAR CharHelper::ToSmall(WCHAR c)
{
return CharToSmall<TCHAR, WCHAR>(c);
}

CHAR CharHelper::ToSmallAnsi(CHAR c)
{
return CharToSmall<CHAR, CHAR>(c);
}

CHAR CharHelper::ToSmallAnsi(WCHAR c)
{
return CharToSmall<CHAR, WCHAR>(c);
}

WCHAR CharHelper::ToSmallUnicode(CHAR c)
{
return CharToSmall<WCHAR, CHAR>(c);
}

WCHAR CharHelper::ToSmallUnicode(WCHAR c)
{
return CharToSmall<WCHAR, WCHAR>(c);
}

WCHAR CharHelper::ToUnicode(CHAR c)
{
BYTE b=(BYTE)c;
if(b<0x80)
	return (WCHAR)c;
b-=0x80;
return UnicodeMap[b];
}

UINT CharHelper::WriteAnsi(OutputStream* stream, CHAR c)
{
if(!stream)
	return sizeof(CHAR);
return (UINT)stream->Write(&c, sizeof(CHAR));
}

UINT CharHelper::WriteAnsi(OutputStream* stream, WCHAR wc)
{
if(!stream)
	return sizeof(CHAR);
CHAR c=CharToChar<CHAR, WCHAR>(wc);
return (UINT)stream->Write(&c, sizeof(CHAR));
}

UINT CharHelper::WriteUnicode(OutputStream* stream, CHAR c)
{
if(!stream)
	return sizeof(WCHAR);
WCHAR wc=CharToChar<WCHAR, CHAR>(c);
return (UINT)stream->Write(&wc, sizeof(WCHAR));
}

UINT CharHelper::WriteUnicode(OutputStream* stream, WCHAR wc)
{
if(!stream)
	return sizeof(WCHAR);
return (UINT)stream->Write(&wc, sizeof(WCHAR));
}

UINT CharHelper::WriteUtf8(OutputStream* stream, CHAR c)
{
return CharWriteUtf8(stream, c);
}

UINT CharHelper::WriteUtf8(OutputStream* stream, WCHAR wc)
{
return CharWriteUtf8(stream, wc);
}
