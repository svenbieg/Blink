//================
// CharHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "CharHelper.h"


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

CHAR CharToAnsi(WCHAR wc)
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

template <class _char_t> _char_t CharToCapital(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c>='a'&&c<='z')
	return CharToChar<_char_t, CHAR>((CHAR)(c-0x20));
if(c==Chars::ae)
	return CharToChar<_char_t, TCHAR>(Chars::AE);
if(c==Chars::oe)
	return CharToChar<_char_t, TCHAR>(Chars::OE);
if(c==Chars::ue)
	return CharToChar<_char_t, TCHAR>(Chars::UE);
return tc;
}

CHAR CharToCapital(CHAR c)
{
return CharToCapital<CHAR>(c);
}

WCHAR CharToCapital(WCHAR c)
{
return CharToCapital<WCHAR>(c);
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

BOOL CharToDigit(CHAR c, UINT* digit_ptr, UINT base)
{
return CharToDigit<CHAR>(c, digit_ptr, base);
}

BOOL CharToDigit(WCHAR c, UINT* digit_ptr, UINT base)
{
return CharToDigit<WCHAR>(c, digit_ptr, base);
}

template <class _char_t> BYTE CharToHex(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c<'0')
	return 0;
BYTE u=(BYTE)(c-'0');
if(u<10)
	return u;
if(c<'A')
	return 0;
u=(BYTE)(c-'A'+10);
if(u<16)
	return u;
if(u<'a')
	return 0;
u=(BYTE)(c-'a'+10);
if(u<16)
	return u;
return 0;
}

BYTE CharToHex(CHAR c)
{
return CharToHex<CHAR>(c);
}

BYTE CharToHex(WCHAR c)
{
return CharToHex<WCHAR>(c);
}

template <class _char_t> _char_t CharToSmall(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c>='A'&&c<='Z')
	return CharToChar<_char_t, CHAR>((CHAR)(c+0x20));
if(c==Chars::AE)
	return CharToChar<_char_t, TCHAR>(Chars::ae);
if(c==Chars::OE)
	return CharToChar<_char_t, TCHAR>(Chars::oe);
if(c==Chars::UE)
	return CharToChar<_char_t, TCHAR>(Chars::ue);
return tc;
}

CHAR CharToSmall(CHAR c)
{
return CharToSmall<CHAR>(c);
}

WCHAR CharToSmall(WCHAR c)
{
return CharToSmall<WCHAR>(c);
}

WCHAR CharToUnicode(CHAR c)
{
BYTE b=(BYTE)c;
if(b<0x80)
	return (WCHAR)c;
b-=0x80;
return UnicodeMap[b];
}


//========
// Common
//========

template <class _char_t> inline BOOL CharIsAlpha(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c>='A'&&c<='Z')
	return true;
if(c>='a'&&c<='z')
	return true;
return false;
}

BOOL CharIsAlpha(CHAR c)
{
return CharIsAlpha<CHAR>(c);
}

BOOL CharIsAlpha(WCHAR c)
{
return CharIsAlpha<WCHAR>(c);
}

template <class _char_t> inline BOOL CharIsBreak(_char_t tc)
{
if(tc==0)
	return true;
CHAR c=CharToChar<CHAR, _char_t>(tc);
CHAR str[]="\n\r\t;:&|+*/\\?!";
for(UINT u=0; u<ArraySize(str); u++)
	{
	if(c==str[u])
		return true;
	}
return false;
}

BOOL CharIsBreak(CHAR c)
{
return CharIsBreak<CHAR>(c);
}

BOOL CharIsBreak(WCHAR c)
{
return CharIsBreak<WCHAR>(c);
}

template <class _char_t> inline BOOL CharIsCapital(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c>='A'&&c<='Z')
	return true;
return false;
}

BOOL CharIsCapital(CHAR c)
{
return CharIsCapital<CHAR>(c);
}

BOOL CharIsCapital(WCHAR c)
{
return CharIsCapital<WCHAR>(c);
}

BOOL CharIsDigit(CHAR c, UINT base)
{
return CharToDigit<CHAR>(c, nullptr, base);
}

BOOL CharIsDigit(WCHAR c, UINT base)
{
return CharToDigit<WCHAR>(c, nullptr, base);
}

template <class _char_t> inline BOOL CharIsPrintable(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c>=' '&&c<='~')
	return true;
return false;
}

BOOL CharIsPrintable(CHAR c)
{
return CharIsPrintable<CHAR>(c);
}

BOOL CharIsPrintable(WCHAR c)
{
return CharIsPrintable<WCHAR>(c);
}

template <class _char_t> inline BOOL CharIsSmall(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
if(c>='a'&&c<='z')
	return true;
return false;
}

BOOL CharIsSmall(CHAR c)
{
return CharIsSmall<CHAR>(c);
}

BOOL CharIsSmall(WCHAR c)
{
return CharIsSmall<WCHAR>(c);
}

template <class _char_t> inline BOOL CharIsSpecial(_char_t tc)
{
CHAR c=CharToChar<CHAR, _char_t>(tc);
CHAR str[]="\"*/:<>?\\|";
for(UINT u=0; u<ArraySize(str); u++)
	{
	if(c==str[u])
		return true;
	}
return false;
}

BOOL CharIsSpecial(CHAR c)
{
return CharIsSpecial<CHAR>(c);
}

BOOL CharIsSpecial(WCHAR c)
{
return CharIsSpecial<WCHAR>(c);
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

INT CharCompare(CHAR c1, CHAR c2, BOOL cs)
{
return CharCompare<CHAR, CHAR>(c1, c2, cs);
}

INT CharCompare(CHAR c1, WCHAR c2, BOOL cs)
{
return CharCompare<CHAR, WCHAR>(c1, c2, cs);
}

INT CharCompare(WCHAR c1, CHAR c2, BOOL cs)
{
return CharCompare<WCHAR, CHAR>(c1, c2, cs);
}

INT CharCompare(WCHAR c1, WCHAR c2, BOOL cs)
{
return CharCompare<WCHAR, WCHAR>(c1, c2, cs);
}

template <class _char1_t, class _char2_t> inline BOOL CharEqual(_char1_t tc1, _char2_t tc2, BOOL cs)
{
CHAR c1=CharToChar<CHAR, _char1_t>(tc1);
CHAR c2=CharToChar<CHAR, _char2_t>(tc2);
if(!cs)
	{
	c1=CharToCapital(c1);
	c2=CharToCapital(c2);
	}
return c1==c2;
}

BOOL CharEqual(CHAR c1, CHAR c2, BOOL cs)
{
return CharEqual<CHAR, CHAR>(c1, c2, cs);
}

BOOL CharEqual(CHAR c1, WCHAR c2, BOOL cs)
{
return CharEqual<CHAR, WCHAR>(c1, c2, cs);
}

BOOL CharEqual(WCHAR c1, CHAR c2, BOOL cs)
{
return CharEqual<WCHAR, CHAR>(c1, c2, cs);
}

BOOL CharEqual(WCHAR c1, WCHAR c2, BOOL cs)
{
return CharEqual<WCHAR, WCHAR>(c1, c2, cs);
}
