//==================
// StringHelper.cpp
//==================

#include "StringHelper.h"


//=======
// Using
//=======

#include "Storage/Streams/StreamWriter.h"
#include "CharHelper.h"
#include "FlagHelper.h"
#include "StringClass.h"
#include <cmath>

using namespace Storage::Streams;


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
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 0x30
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


//============
// Encryption
//============

constexpr UINT ENCRYPT_BLOCK=16;
constexpr UINT ENCRYPT_LOOPS=16;

constexpr BYTE ENCRYPT_FWD[256]=
	{
	0x39, 0x47, 0x77, 0x09, 0x64, 0x20, 0x69, 0x94, 0x42, 0xE0, 0x0A, 0x36, 0x6D, 0x2B, 0x7A, 0x12,
	0x23, 0x24, 0x5B, 0x57, 0x85, 0x22, 0x62, 0x26, 0xE3, 0xFC, 0x25, 0x84, 0xF7, 0x11, 0x0D, 0x3F,
	0x52, 0x4A, 0x9F, 0xD1, 0xE7, 0xF0, 0x0E, 0xBE, 0xF4, 0xD7, 0x31, 0x2F, 0xD2, 0x66, 0xBB, 0xCE,
	0xF9, 0x6E, 0xED, 0x1D, 0x9C, 0x78, 0x4E, 0xD5, 0xFE, 0xF1, 0xAC, 0x2A, 0x21, 0x70, 0x7B, 0x38,
	0xA4, 0x06, 0x9B, 0xD9, 0x6F, 0xA6, 0xB9, 0xEB, 0x3D, 0xBF, 0xDA, 0x3A, 0x56, 0x28, 0xE8, 0x97,
	0xC7, 0x71, 0x00, 0x7D, 0x2E, 0xBC, 0x6C, 0xE1, 0x60, 0x29, 0x65, 0x13, 0x82, 0x03, 0x0B, 0xB4,
	0xDE, 0x14, 0xA1, 0xB2, 0xDC, 0xE4, 0x40, 0x79, 0xEC, 0xAE, 0x3B, 0x49, 0x8F, 0xEE, 0xCB, 0x87,
	0x44, 0xC3, 0xE9, 0x41, 0x88, 0xB5, 0xA0, 0x35, 0x74, 0xDB, 0xF5, 0x99, 0xA9, 0xC9, 0xFD, 0xD8,
	0x86, 0x1A, 0x45, 0xCD, 0x15, 0xD6, 0x37, 0x8D, 0x8E, 0xB6, 0xD0, 0xF8, 0xE5, 0xEA, 0x59, 0xC1,
	0x04, 0xFF, 0xAD, 0x61, 0x67, 0xFB, 0x18, 0xF3, 0x16, 0x6B, 0x5C, 0x53, 0x81, 0x8A, 0xAA, 0x30,
	0x7F, 0x07, 0x0C, 0x9A, 0x4D, 0xB8, 0x1B, 0x76, 0x96, 0x4C, 0x90, 0xDD, 0x5E, 0x8B, 0xE6, 0xCA,
	0xC0, 0xCC, 0x05, 0x73, 0xEF, 0x43, 0xE2, 0x75, 0x54, 0xA3, 0x1F, 0xC6, 0x6A, 0x33, 0x7E, 0x93,
	0x3C, 0x55, 0x92, 0xA8, 0x08, 0x80, 0x01, 0x98, 0xD4, 0x19, 0xAF, 0x7C, 0x2C, 0x27, 0xC2, 0x1E,
	0x34, 0x58, 0x8C, 0x2D, 0x9D, 0xC5, 0x68, 0x02, 0x63, 0x5D, 0x72, 0xFA, 0x10, 0x3E, 0xDF, 0x17,
	0x46, 0x5A, 0xA7, 0x83, 0x9E, 0x48, 0x4F, 0xC8, 0x0F, 0xB1, 0x5F, 0x95, 0xBD, 0x32, 0x91, 0xCF,
	0x50, 0x89, 0xAB, 0x4B, 0xF6, 0xF2, 0xA2, 0xB0, 0xB7, 0x51, 0xBA, 0x1C, 0xD3, 0xB3, 0xC4, 0xA5
	};

constexpr BYTE ENCRYPT_BACK[256]=
	{
	0x52, 0xC6, 0xD7, 0x5D, 0x90, 0xB2, 0x41, 0xA1, 0xC4, 0x03, 0x0A, 0x5E, 0xA2, 0x1E, 0x26, 0xE8,
	0xDC, 0x1D, 0x0F, 0x5B, 0x61, 0x84, 0x98, 0xDF, 0x96, 0xC9, 0x81, 0xA6, 0xFB, 0x33, 0xCF, 0xBA,
	0x05, 0x3C, 0x15, 0x10, 0x11, 0x1A, 0x17, 0xCD, 0x4D, 0x59, 0x3B, 0x0D, 0xCC, 0xD3, 0x54, 0x2B,
	0x9F, 0x2A, 0xED, 0xBD, 0xD0, 0x77, 0x0B, 0x86, 0x3F, 0x00, 0x4B, 0x6A, 0xC0, 0x48, 0xDD, 0x1F,
	0x66, 0x73, 0x08, 0xB5, 0x70, 0x82, 0xE0, 0x01, 0xE5, 0x6B, 0x21, 0xF3, 0xA9, 0xA4, 0x36, 0xE6,
	0xF0, 0xF9, 0x20, 0x9B, 0xB8, 0xC1, 0x4C, 0x13, 0xD1, 0x8E, 0xE1, 0x12, 0x9A, 0xD9, 0xAC, 0xEA,
	0x58, 0x93, 0x16, 0xD8, 0x04, 0x5A, 0x2D, 0x94, 0xD6, 0x06, 0xBC, 0x99, 0x56, 0x0C, 0x31, 0x44,
	0x3D, 0x51, 0xDA, 0xB3, 0x78, 0xB7, 0xA7, 0x02, 0x35, 0x67, 0x0E, 0x3E, 0xCB, 0x53, 0xBE, 0xA0,
	0xC5, 0x9C, 0x5C, 0xE3, 0x1B, 0x14, 0x80, 0x6F, 0x74, 0xF1, 0x9D, 0xAD, 0xD2, 0x87, 0x88, 0x6C,
	0xAA, 0xEE, 0xC2, 0xBF, 0x07, 0xEB, 0xA8, 0x4F, 0xC7, 0x7B, 0xA3, 0x42, 0x34, 0xD4, 0xE4, 0x22,
	0x76, 0x62, 0xF6, 0xB9, 0x40, 0xFF, 0x45, 0xE2, 0xC3, 0x7C, 0x9E, 0xF2, 0x3A, 0x92, 0x69, 0xCA,
	0xF7, 0xE9, 0x63, 0xFD, 0x5F, 0x75, 0x89, 0xF8, 0xA5, 0x46, 0xFA, 0x2E, 0x55, 0xEC, 0x27, 0x49,
	0xB0, 0x8F, 0xCE, 0x71, 0xFE, 0xD5, 0xBB, 0x50, 0xE7, 0x7D, 0xAF, 0x6E, 0xB1, 0x83, 0x2F, 0xEF,
	0x8A, 0x23, 0x2C, 0xFC, 0xC8, 0x37, 0x85, 0x29, 0x7F, 0x43, 0x4A, 0x79, 0x64, 0xAB, 0x60, 0xDE,
	0x09, 0x57, 0xB6, 0x18, 0x65, 0x8C, 0xAE, 0x24, 0x4E, 0x72, 0x8D, 0x47, 0x68, 0x32, 0x6D, 0xB4,
	0x25, 0x39, 0xF5, 0x97, 0x28, 0x7A, 0xF4, 0x1C, 0x8B, 0x30, 0xDB, 0x95, 0x19, 0x7E, 0x38, 0x91
	};

VOID EncryptGenerateKey(BYTE* key, LPCSTR str)
{
if(!str)
	throw InvalidArgumentException();
UINT pos=0;
UINT len=0;
while(str[len])
	key[pos++%ENCRYPT_BLOCK]^=ENCRYPT_FWD[(SIZE_T)str[len++]];
while(pos<ENCRYPT_BLOCK)
	key[pos++]=ENCRYPT_FWD[key[pos-len]];
}

VOID EncryptShiftLeft(BYTE* buf)
{
BYTE b0=buf[0];
for(UINT u=1; u<ENCRYPT_BLOCK; u++)
	buf[u-1]=buf[u];
buf[ENCRYPT_BLOCK-1]=b0;
}

VOID EncryptShiftRight(BYTE* buf)
{
BYTE bn=buf[ENCRYPT_BLOCK-1];
for(UINT u=1; u<ENCRYPT_BLOCK; u++)
	buf[ENCRYPT_BLOCK-u]=buf[ENCRYPT_BLOCK-u-1];
buf[0]=bn;
}


//========
// Floats
//========

template <class _float_t> INT FloatNormalize(_float_t& f, _float_t max, _float_t min)
{
if(f==0)
	return 0;
constexpr _float_t exps[]={  1e32f,  1e16f,  1e8f,  1e4f,  1e2f, 1e1f };
constexpr _float_t negs[]={ 1e-31f, 1e-15f, 1e-7f, 1e-3f, 1e-1f, 1e0f };
INT ex=0;
INT num=32;
if(f>=max)
	{
	for(UINT u=0; u<TypeHelper::ArraySize(exps); u++)
		{
		if(f>=exps[u])
			{
			f/=exps[u];
			ex+=num;
			}
		num/=2;
		}
	return ex;
	}
if(f<min)
	{
	for(UINT u=0; u<TypeHelper::ArraySize(exps); u++)
		{
		if(f<negs[u])
			{
			f*=exps[u];
			ex-=num;
			}
		num/=2;
		}
	}
return ex;
}

template <> INT FloatNormalize<DOUBLE>(DOUBLE& f, DOUBLE max, DOUBLE min)
{
if(f==0)
	return 0;
constexpr DOUBLE exps[]={  1e256,  1e128,  1e64,  1e32,  1e16,  1e8,  1e4,  1e2, 1e1 };
constexpr DOUBLE negs[]={ 1e-255, 1e-127, 1e-63, 1e-31, 1e-15, 1e-7, 1e-3, 1e-1, 1e0 };
INT ex=0;
INT num=256;
if(f>=max)
	{
	for(UINT u=0; u<TypeHelper::ArraySize(exps); u++)
		{
		if(f>=exps[u])
			{
			f/=exps[u];
			ex+=num;
			}
		num/=2;
		}
	return ex;
	}
if(f<min)
	{
	for(UINT u=0; u<TypeHelper::ArraySize(exps); u++)
		{
		if(f<negs[u])
			{
			f*=exps[u];
			ex-=num;
			}
		num/=2;
		}
	}
return ex;
}

template <class _float_t> VOID FloatSplit(_float_t f, UINT& integral, UINT& decimal, INT& exponent, UINT& width, UINT& precision)
{
_float_t mul[]={ 1e0f, 1e1f, 1e2f, 1e3f, 1e4f, 1e5f, 1e6f, 1e7f, 1e8f, 1e9f };
_float_t div[]={ 1e0f, 1e-1f, 1e-2f, 1e-3f, 1e-4f, 1e-5f };
UINT sel=TypeHelper::Min(width, TypeHelper::ArraySize(mul)-1);
UINT prec=TypeHelper::Min(precision, TypeHelper::ArraySize(div)-1);
exponent=FloatNormalize<_float_t>(f, mul[sel], div[prec]);
integral=(UINT)f;
_float_t remainder=f-(_float_t)integral;
remainder*=mul[prec];
decimal=(UINT)remainder;
remainder-=(_float_t)decimal;
if(remainder>=0.5)
	{
	decimal++;
	if(decimal==1000000000)
		{
		integral++;
		decimal=0;
		if((_float_t)integral>=mul[sel])
			{
			exponent++;
			integral/=10;
			}
		}
	}
}


//========
// Common
//========

UINT StringHelper::Append(LPSTR dst, UINT size, LPCSTR src, LPCSTR value)
{
return Append<CHAR, CHAR, CHAR>(dst, size, src, value);
}

UINT StringHelper::Append(LPWSTR dst, UINT size, LPCSTR src, LPCSTR value)
{
return Append<WCHAR, CHAR, CHAR>(dst, size, src, value);
}

INT StringHelper::Compare(LPCSTR str1, LPCSTR str2, UINT len, BOOL cs)
{
return Compare<CHAR, CHAR>(str1, str2, len, cs);
}

INT StringHelper::Compare(LPCSTR str1, LPCWSTR str2, UINT len, BOOL cs)
{
return Compare<CHAR, WCHAR>(str1, str2, len, cs);
}

INT StringHelper::Compare(LPCWSTR str1, LPCSTR str2, UINT len, BOOL cs)
{
return Compare<WCHAR, CHAR>(str1, str2, len, cs);
}

INT StringHelper::Compare(LPCWSTR str1, LPCWSTR str2, UINT len, BOOL cs)
{
return Compare<WCHAR, WCHAR>(str1, str2, len, cs);
}

INT StringHelper::Compare(LPCSTR str1, String const* str2, UINT len, BOOL cs)
{
return Compare<CHAR>(str1, str2, len, cs);
}

INT StringHelper::Compare(LPCWSTR str1, String const* str2, UINT len, BOOL cs)
{
return Compare<WCHAR>(str1, str2, len, cs);
}

INT StringHelper::Compare(String const* str1, LPCSTR str2, UINT len, BOOL cs)
{
return Compare<CHAR>(str1, str2, len, cs);
}

INT StringHelper::Compare(String const* str1, LPCWSTR str2, UINT len, BOOL cs)
{
return Compare<WCHAR>(str1, str2, len, cs);
}

INT StringHelper::Compare(String const* str1, String const* str2)
{
if(!str1)
	{
	if(!str2)
		return 0;
	return -1;
	}
if(!str2)
	return 1;
UINT64 hash1=str1->m_Hash;
UINT64 hash2=str2->m_Hash;
if(hash1<hash2)
	return -1;
if(hash1>hash2)
	return 1;
return Compare(str1->Begin(), str2->Begin(), 0, false);
}

INT StringHelper::Compare(String const* str1, String const* str2, UINT len, BOOL cs)
{
return Compare(str1? str1->Begin(): nullptr, str2? str2->Begin(): nullptr, len, cs);
}

UINT StringHelper::Copy(LPSTR dst, UINT size, LPCSTR src, UINT len)
{
return Copy<CHAR, CHAR>(dst, size, src, len);
}

UINT StringHelper::Copy(LPSTR dst, UINT size, LPCWSTR src, UINT len)
{
return Copy<CHAR, WCHAR>(dst, size, src, len);
}

UINT StringHelper::Copy(LPWSTR dst, UINT size, LPCSTR src, UINT len)
{
return Copy<WCHAR, CHAR>(dst, size, src, len);
}

UINT StringHelper::Copy(LPWSTR dst, UINT size, LPCWSTR src, UINT len)
{
return Copy<WCHAR, WCHAR>(dst, size, src, len);
}

UINT StringHelper::Decrypt(LPSTR dst, UINT size, BYTE const* src, LPCSTR key_str)
{
if(!src)
	return 0;
BYTE key[ENCRYPT_BLOCK]={ 0 };
EncryptGenerateKey(key, key_str);
UINT src_pos=0;
UINT len=0;
while(1)
	{
	BYTE buf[ENCRYPT_BLOCK];
	MemoryHelper::Copy(buf, &src[src_pos], ENCRYPT_BLOCK);
	for(UINT loop=0; loop<ENCRYPT_LOOPS; loop++)
		{
		EncryptShiftRight(buf);
		for(UINT u=0; u<ENCRYPT_BLOCK; u++)
			buf[u]=ENCRYPT_BACK[buf[u]]^key[u];
		}
	for(UINT u=0; u<ENCRYPT_BLOCK; u++)
		{
		if(dst)
			dst[len]=buf[u];
		if(!buf[u])
			return len;
		len++;
		}
	src_pos+=ENCRYPT_BLOCK;
	}
return len;
}

UINT StringHelper::Encrypt(BYTE* dst, SIZE_T size, LPCSTR src, LPCSTR key_str)
{
if(!src||!src[0])
	return 0;
if(!dst)
	{
	UINT len=Length<CHAR>(src);
	return TypeHelper::AlignUp(len, ENCRYPT_BLOCK);
	}
BYTE key[ENCRYPT_BLOCK]={ 0 };
EncryptGenerateKey(key, key_str);
UINT src_pos=0;
UINT dst_pos=0;
while(src[src_pos])
	{
	BYTE buf[ENCRYPT_BLOCK]={ 0 };
	for(UINT u=0; u<ENCRYPT_BLOCK; u++)
		{
		if(!src[src_pos])
			break;
		buf[u]=src[src_pos++];
		}
	for(UINT loop=0; loop<ENCRYPT_LOOPS; loop++)
		{
		for(UINT u=0; u<ENCRYPT_BLOCK; u++)
			buf[u]=ENCRYPT_FWD[buf[u]^key[u]];
		EncryptShiftLeft(buf);
		}
	if(dst_pos+ENCRYPT_BLOCK>size)
		throw BufferOverrunException();
	MemoryHelper::Copy(&dst[dst_pos], buf, ENCRYPT_BLOCK);
	dst_pos+=ENCRYPT_BLOCK;
	}
return dst_pos;
}

BOOL StringHelper::FindChar(LPCSTR str, CHAR c, UINT* pos, BOOL cs)
{
return FindChar<CHAR, CHAR>(str, c, pos, cs);
}

BOOL StringHelper::FindChar(LPCSTR str, WCHAR c, UINT* pos, BOOL cs)
{
return FindChar<CHAR, WCHAR>(str, c, pos, cs);
}

BOOL StringHelper::FindChar(LPCWSTR str, CHAR c, UINT* pos, BOOL cs)
{
return FindChar<WCHAR, CHAR>(str, c, pos, cs);
}

BOOL StringHelper::FindChar(LPCWSTR str, WCHAR c, UINT* pos, BOOL cs)
{
return FindChar<WCHAR, WCHAR>(str, c, pos, cs);
}

BOOL StringHelper::FindChars(LPCSTR str, LPCSTR find, UINT* pos, BOOL cs)
{
return FindChars<CHAR, CHAR>(str, find, pos, cs);
}

BOOL StringHelper::FindChars(LPCSTR str, LPCWSTR find, UINT* pos, BOOL cs)
{
return FindChars<CHAR, WCHAR>(str, find, pos, cs);
}

BOOL StringHelper::FindChars(LPCWSTR str, LPCSTR find, UINT* pos, BOOL cs)
{
return FindChars<WCHAR, CHAR>(str, find, pos, cs);
}

BOOL StringHelper::FindChars(LPCWSTR str, LPCWSTR find, UINT* pos, BOOL cs)
{
return FindChars<WCHAR, WCHAR>(str, find, pos, cs);
}

BOOL StringHelper::FindString(LPCSTR str, LPCSTR find, UINT* pos, BOOL cs)
{
return FindString<CHAR, CHAR>(str, find, pos, cs);
}

BOOL StringHelper::FindString(LPCSTR str, LPCWSTR find, UINT* pos, BOOL cs)
{
return FindString<CHAR, WCHAR>(str, find, pos, cs);
}

BOOL StringHelper::FindString(LPCWSTR str, LPCSTR find, UINT* pos, BOOL cs)
{
return FindString<WCHAR, CHAR>(str, find, pos, cs);
}

BOOL StringHelper::FindString(LPCWSTR str, LPCWSTR find, UINT* pos, BOOL cs)
{
return FindString<WCHAR, WCHAR>(str, find, pos, cs);
}

UINT64 StringHelper::Hash(LPCSTR str)
{
return Hash<CHAR>(str);
}

UINT64 StringHelper::Hash(LPCWSTR str)
{
return Hash<WCHAR>(str);
}

UINT StringHelper::Insert(LPSTR dst, UINT size, LPCSTR src, UINT pos, LPCSTR value)
{
return Insert<CHAR, CHAR, CHAR>(dst, size, src, pos, value);
}

UINT StringHelper::Insert(LPWSTR dst, UINT size, LPCWSTR src, UINT pos, LPCSTR value)
{
return Insert<WCHAR, WCHAR, CHAR>(dst, size, src, pos, value);
}

UINT StringHelper::Length(LPCSTR str)
{
return Length<CHAR>(str);
}

UINT StringHelper::Length(LPCWSTR str)
{
return Length<WCHAR>(str);
}

UINT StringHelper::Length(LPCSTR str, UINT max)
{
return Length<CHAR>(str, max);
}

UINT StringHelper::Length(LPCWSTR str, UINT max)
{
return Length<WCHAR>(str, max);
}

UINT StringHelper::Length(LPCSTR format, VariableArguments& args)
{
return Print((LPSTR)nullptr, 0, format, args);
}

UINT StringHelper::Lowercase(LPSTR str)
{
return Lowercase<CHAR>(str);
}

UINT StringHelper::Lowercase(LPWSTR str)
{
return Lowercase<WCHAR>(str);
}

UINT StringHelper::Lowercase(LPSTR dst, UINT size, LPCSTR src)
{
return Lowercase<CHAR, CHAR>(dst, size, src);
}

UINT StringHelper::Lowercase(LPWSTR dst, UINT size, LPCWSTR src)
{
return Lowercase<WCHAR, WCHAR>(dst, size, src);
}

UINT StringHelper::Print(LPSTR dst, UINT size, LPCSTR format, VariableArguments& args)
{
return Print<CHAR>(dst, size, format, args);
}

UINT StringHelper::Print(LPWSTR dst, UINT size, LPCSTR format, VariableArguments& args)
{
return Print<WCHAR>(dst, size, format, args);
}

UINT StringHelper::Replace(LPSTR dst, UINT size, LPCSTR src, LPCSTR find, LPCSTR insert, BOOL cs, BOOL repeat)
{
return Replace<CHAR, CHAR, CHAR>(dst, size, src, find, insert, cs, repeat);
}

UINT StringHelper::Replace(LPWSTR dst, UINT size, LPCWSTR src, LPCSTR find, LPCSTR insert, BOOL cs, BOOL repeat)
{
return Replace<WCHAR, WCHAR, CHAR>(dst, size, src, find, insert, cs, repeat);
}

UINT StringHelper::Scan(LPCSTR str, LPCSTR format, VariableArguments& args)
{
return Scan<CHAR>(str, format, args);
}

UINT StringHelper::Scan(LPCWSTR str, LPCSTR format, VariableArguments& args)
{
return Scan<WCHAR>(str, format, args);
}

LPCSTR StringHelper::Truncate(LPCSTR str, LPCSTR chars)
{
return Truncate<CHAR>(str, chars);
}

LPCWSTR StringHelper::Truncate(LPCWSTR str, LPCSTR chars)
{
return Truncate<WCHAR>(str, chars);
}

UINT StringHelper::Uppercase(LPSTR str)
{
return Uppercase<CHAR>(str);
}

UINT StringHelper::Uppercase(LPWSTR str)
{
return Uppercase<WCHAR>(str);
}

UINT StringHelper::Uppercase(LPSTR dst, UINT size, LPCSTR src)
{
return Uppercase<CHAR, CHAR>(dst, size, src);
}

UINT StringHelper::Uppercase(LPWSTR dst, UINT size, LPCWSTR src)
{
return Uppercase<WCHAR, WCHAR>(dst, size, src);
}


//================
// Common Private
//================

template <std::character _dst_t, std::character _src_t, std::character _append_t> UINT StringHelper::Append(_dst_t* dst, UINT size, _src_t const* str, _append_t const* append)
{
UINT len=Length(str);
if(!len)
	return Copy(dst, size, append);
UINT append_len=Length(append);
if(!append_len)
	return Copy(dst, size, str);
UINT new_len=len+append_len;
if(!size)
	return new_len;
if(new_len+1>size)
	return 0;
size-=Copy(dst, size, str);
Copy(&dst[len], size, append);
return new_len;
}

template <std::character _str1_t, std::character _str2_t> INT StringHelper::Compare(_str1_t const* str1, _str2_t const* str2, UINT count, BOOL cs)
{
if(!str1)
	{
	if(!str2)
		return 0;
	if(!str2[0])
		return 0;
	return -1;
	}
if(!str2)
	{
	if(!str1[0])
		return 0;
	return 1;
	}
UINT pos1=0;
UINT pos2=0;
while(str1[pos1]&&str2[pos2])
	{
	FLOAT f1=0;
	UINT len1=ScanFloat(&str1[pos1], &f1);
	FLOAT f2=0;
	UINT len2=ScanFloat(&str2[pos2], &f2);
	if(len1==0)
		{
		if(len2==0)
			{
			INT cmp=CharHelper::Compare(str1[pos1], str2[pos2], cs);
			if(cmp==0)
				{
				pos1++;
				pos2++;
				if(pos2==count)
					return 0;
				continue;
				}
			return cmp;
			}
		return -1;
		}
	if(len2==0)
		return 1;
	if(f1>f2)
		return 1;
	if(f2>f1)
		return -1;
	pos1+=len1;
	pos2+=len2;
	}
if(str1[pos1]==0)
	{
	if(str2[pos2]==0)
		return 0;
	return -1;
	}
return 1;
}

template <std::character _str_t> INT StringHelper::Compare(String const* str, _str_t const* value2, UINT len, BOOL cs)
{
auto value1=str? str->Begin(): nullptr;
return Compare(value1, value2, len, cs);
}

template <std::character _str_t> INT StringHelper::Compare(_str_t const* value1, String const* str, UINT len, BOOL cs)
{
auto value2=str? str->Begin(): nullptr;
return Compare(value1, value2, len, cs);
}

template <std::character _dst_t, std::character _src_t> UINT StringHelper::Copy(_dst_t* dst, UINT size, _src_t const* src, UINT len)
{
if(!dst)
	return 0;
if(!src)
	{
	if(dst)
		dst[0]=0;
	return 0;
	}
if(size==0)
	size=UINT_MAX;
UINT end=size-1;
if(len)
	end=TypeHelper::Min(end, len);
UINT pos=0;
for(; pos<end; pos++)
	{
	if(src[pos]==0)
		break;
	if(dst)
		dst[pos]=CharHelper::ToChar<_dst_t, _src_t>(src[pos]);
	}
if(dst)
	dst[pos]=0;
return pos;
}

template <std::character _str_t, std::character _find_t> BOOL StringHelper::FindChar(_str_t const* str, _find_t c, UINT* pos_ptr, BOOL cs)
{
if(!str||!c)
	return false;
BOOL found=false;
UINT pos=0;
for(; str[pos]; pos++)
	{
	if(CharHelper::Compare(str[pos], c, cs)==0)
		{
		found=true;
		break;
		}
	}
if(pos_ptr)
	*pos_ptr=pos;
return found;
}

template <std::character _str_t, std::character _find_t> BOOL StringHelper::FindChars(_str_t const* str, _find_t const* find, UINT* pos_ptr, BOOL cs)
{
if(!str||!find)
	return false;
UINT pos=0;
for(; str[pos]; pos++)
	{
	for(UINT find_pos=0; find[find_pos]; find_pos++)
		{
		if(CharHelper::Compare(str[pos], find[find_pos], cs)==0)
			{
			if(pos_ptr)
				*pos_ptr=pos;
			return true;
			}
		}
	}
if(pos_ptr)
	*pos_ptr=pos;
return false;
}

template <std::character _str_t, std::character _find_t> BOOL StringHelper::FindString(_str_t const* str, _find_t const* find, UINT* pos_ptr, BOOL cs)
{
if(!str||!find)
	return false;
UINT find_len=Length<_find_t>(find);
if(!find_len)
	return false;
for(UINT pos=0; str[pos]; pos++)
	{
	if(Compare<_str_t, _find_t>(&str[pos], find, find_len, cs)==0)
		{
		if(pos_ptr)
			*pos_ptr=pos;
		return true;
		}
	}
return false;
}

UINT StringHelper::GetFormat(LPCSTR str, Format& format, FormatFlags& flags, UINT& width, UINT& precision)
{
if(!str)
	return 0;
if(!CharHelper::Equal(str[0], '%'))
	return 0;
if(str[1]==0)
	{
	format=Format::Percent;
	return 1;
	}
if(CharHelper::Equal(str[1], '%'))
	{
	format=Format::Percent;
	return 2;
	}
// Flags
UINT pos=1;
for(; str[pos]; pos++)
	{
	if(CharHelper::Equal(str[pos], ' '))
		{
		FlagHelper::Set(flags, FormatFlags::Space);
		}
	else if(CharHelper::Equal(str[pos], '+'))
		{
		FlagHelper::Set(flags, FormatFlags::Signed);
		}
	else if(CharHelper::Equal(str[pos], '-'))
		{
		FlagHelper::Set(flags, FormatFlags::Left);
		}
	else if(CharHelper::Equal(str[pos], '#'))
		{
		FlagHelper::Set(flags, FormatFlags::Numeric);
		}
	else if(CharHelper::Equal(str[pos], '0'))
		{
		if(FlagHelper::Get(flags, FormatFlags::Zero))
			break;
		FlagHelper::Set(flags, FormatFlags::Zero);
		}
	else
		{
		break;
		}
	}
// Width
if(CharHelper::Equal(str[pos], '*'))
	{
	FlagHelper::Set(flags, FormatFlags::Width);
	pos++;
	}
else
	{
	pos+=ScanUInt(&str[pos], &width);
	}
// Precision
if(CharHelper::Equal(str[pos], '.'))
	{
	pos++;
	if(CharHelper::Equal(str[pos], '*'))
		{
		FlagHelper::Set(flags, FormatFlags::Precision);
		pos++;
		}
	else
		{
		UINT len=ScanUInt(&str[pos], &precision);
		if(len==0)
			precision=0;
		pos+=len;
		}
	}
// size
if(CharHelper::Equal(str[pos], 'h'))
	{
	FlagHelper::Set(flags, FormatFlags::High);
	pos++;
	}
else if(CharHelper::Equal(str[pos], 'l'))
	{
	FlagHelper::Set(flags, FormatFlags::Low);
	pos++;
	}
// Type
CHAR c=str[pos];
if(c==0)
	{
	format=Format::None;
	return pos;
	}
switch(c)
	{
	case 'c':
		{
		format=Format::Char;
		break;
		}
	case 'C':
		{
		FlagHelper::Set(flags, FormatFlags::High);
		format=Format::Char;
		break;
		}
	case 'd':
		{
		format=Format::Double;
		break;
		}
	case 'f':
		{
		format=Format::Float;
		break;
		}
	case 'i':
		{
		format=Format::Int;
		break;
		}
	case 's':
		{
		format=Format::String;
		break;
		}
	case 'S':
		{
		FlagHelper::Set(flags, FormatFlags::High);
		format=Format::String;
		break;
		}
	case 'u':
		{
		format=Format::UInt;
		break;
		}
	case 'x':
		{
		format=Format::Hex;
		break;
		}
	default:
		{
		format=Format::None;
		break;
		}
	}
return pos+1;
}

template <std::character _str_t> UINT64 StringHelper::Hash(_str_t const* str)
{
if(!str)
	return 0;
UINT64 hash=0;
UINT pos=0;
for(; pos<CHARS_PER_HASH; pos++)
	{
	auto c=CharHelper::ToAnsi(str[pos]);
	if(!c)
		break;
	auto code=HASH_CODE[(BYTE)c];
	hash<<=BITS_PER_CHAR;
	hash|=code;
	}
hash<<=((CHARS_PER_HASH-pos)*BITS_PER_CHAR);
return hash;
}

template <std::character _dst_t, std::character _src_t, std::character _insert_t> UINT StringHelper::Insert(_dst_t* dst, UINT size, _src_t const* str, UINT pos, _insert_t const* insert)
{
UINT len=Length(str);
if(pos>len)
	return Copy(dst, size, str);
UINT insert_len=Length(insert);
if(insert_len==0)
	return Copy(dst, size, str);
UINT new_len=len+insert_len;
if(!size)
	return new_len;
if(new_len+1>size)
	return 0;
size-=Copy(dst, size, str, pos);
size-=Copy(&dst[pos], size, insert);
Copy(&dst[pos+insert_len], size, &str[pos]);
return new_len;
}

template <std::character _str_t> UINT StringHelper::Length(_str_t const* value)
{
if(!value)
	return 0;
UINT len=0;
for(; value[len]; len++);
return len;
}

template <std::character _str_t> UINT StringHelper::Length(_str_t const* value, UINT max)
{
if(!value)
	return 0;
if(!max)
	max=UINT_MAX;
UINT len=0;
for(; value[len]; len++)
	{
	if(len==max)
		break;
	}
return len;
}

template <std::character _str_t> UINT StringHelper::Lowercase(_str_t* str)
{
if(!str)
	return 0;
UINT pos=0;
for(; str[pos]; pos++)
	str[pos]=CharHelper::ToSmall<_str_t, _str_t>(str[pos]);
return pos;
}

template <std::character _dst_t, std::character _src_t> UINT StringHelper::Lowercase(_dst_t* dst, UINT size, _src_t const* src)
{
if(!dst)
	return 0;
UINT pos=0;
for(; dst[pos]; pos++)
	{
	if(pos+1==size)
		break;
	if(dst)
		dst[pos]=CharHelper::ToSmall<_dst_t, _src_t>(src[pos]);
	}
if(dst)
	dst[pos]=0;
return pos;
}

template <std::character _str_t> UINT StringHelper::Print(_str_t* str, UINT size, LPCSTR format, VariableArguments& args)
{
if(!format)
	return 0;
args.Reset();
UINT pos=0;
for(UINT fmt=0; format[fmt]; )
	{
	if(pos+1==size)
		break;
	Format str_format=Format::None;
	FormatFlags flags=FormatFlags::None;
	UINT width=0;
	UINT prec=UINT_MAX;
	fmt+=GetFormat(&format[fmt], str_format, flags, width, prec);
	if(str_format==Format::None)
		{
		pos+=PrintChar(str, size, format[fmt++], pos);
		continue;
		}
	if(str_format==Format::Percent)
		{
		pos+=PrintChar(str, size, '%', pos);
		continue;
		}
	if(FlagHelper::Get(flags, FormatFlags::Width))
		{
		if(!args.Get(width))
			break;
		}
	if(FlagHelper::Get(flags, FormatFlags::Precision))
		{
		if(!args.Get(prec))
			break;
		}
	switch(str_format)
		{
		case Format::Int:
			{
			INT64 i=0;
			if(!args.Get(i))
				break;
			pos+=PrintInt(str, size, i, flags, width, pos);
			continue;
			}
		case Format::UInt:
			{
			UINT64 u=0;
			if(!args.Get(u))
				break;
			pos+=PrintUInt(str, size, u, flags, width, pos);
			continue;
			}
		case Format::Hex:
			{
			UINT64 u=0;
			if(!args.Get(u))
				break;
			pos+=PrintHex(str, size, u, flags, width, pos);
			continue;
			}
		case Format::Float:
			{
			FLOAT f=0;
			if(!args.Get(f))
				break;
			pos+=PrintFloat(str, size, f, flags, width, prec, pos);
			continue;
			}
		case Format::Double:
			{
			DOUBLE d=0;
			if(!args.Get(d))
				break;
			pos+=PrintFloat(str, size, d, flags, width, prec, pos);
			continue;
			}
		case Format::Char:
			{
			WCHAR c=' ';
			if(!args.Get(c))
				break;
			pos+=PrintChar(str, size, c, flags, pos);
			continue;
			}
		case Format::String:
			{
			LPCSTR p=nullptr;
			if(args.Get(p))
				{
				pos+=PrintString(str, size, p, flags, width, pos);
				continue;
				}
			LPCWSTR pw=nullptr;
			if(args.Get(pw))
				{
				pos+=PrintString(str, size, pw, flags, width, pos);
				continue;
				}
			break;
			}
		default:
			break;
		}
	break;
	}
if(pos<size)
	str[pos]=0;
return pos;
}

template <std::character _dst_t, std::character _char_t> UINT StringHelper::PrintChar(_dst_t* dst, UINT size, _char_t c, UINT pos)
{
if(!dst)
	return 1;
if(pos>=size)
	return 0;
dst[pos]=CharHelper::ToChar<_dst_t, _char_t>(c);
return 1;
}

template <std::character _dst_t, std::character _char_t> UINT StringHelper::PrintChar(_dst_t* dst, UINT size, _char_t c, FormatFlags flags, UINT pos)
{
if(!dst)
	return 1;
if(pos>=size)
	return 0;
if(FlagHelper::Get(flags, FormatFlags::High))
	{
	dst[pos]=CharHelper::ToCapital<_dst_t, _char_t>(c);
	}
else if(FlagHelper::Get(flags, FormatFlags::Low))
	{
	dst[pos]=CharHelper::ToSmall<_dst_t, _char_t>(c);
	}
else
	{
	dst[pos]=CharHelper::ToChar<_dst_t, _char_t>(c);
	}
return 1;
}

template <std::character _dst_t, std::character _char_t> UINT StringHelper::PrintChars(_dst_t* dst, UINT size, _char_t c, UINT count, UINT pos)
{
if(!dst)
	return count;
if(pos>=size)
	return 0;
UINT print=TypeHelper::Min(size-pos, count);
for(UINT u=0; u<print; u++)
	dst[pos++]=CharHelper::ToChar<_dst_t, _char_t>(c);
return print;
}

template <std::character _dst_t, std::floating_point _float_t> UINT StringHelper::PrintFloat(_dst_t* dst, UINT size, _float_t f, FormatFlags flags, UINT width, UINT precision, UINT pos)
{
UINT start=pos;
if(std::isnan(f))
	{
	pos+=PrintString(dst, size, "nan", pos);
	return pos-start;
	}
if(f<0)
	{
	pos+=PrintChar(dst, size, '-', pos);
	f*=-1;
	}
else if(FlagHelper::Get(flags, FormatFlags::Signed))
	{
	pos+=PrintChar(dst, size, '+', pos);
	}
if(std::isinf(f))
	{
	pos+=PrintString(dst, size, "inf", pos);
	return pos-start;
	}
UINT integral=0;
UINT decimal=0;
INT exponent=0;
FloatSplit(f, integral, decimal, exponent, width, precision);
pos+=PrintUInt(dst, size, integral, pos);
CHAR chars[16];
LPSTR buf=&chars[15];
*buf=0;
INT idec=precision;
for(; idec>1&&decimal%10==0; idec--)
	{
	if(FlagHelper::Get(flags, FormatFlags::Zero))
		*--buf='0';
	decimal/=10;
	}
for(; idec>0; idec--)
	{
	*--buf=(CHAR)((decimal%10)+'0');
	decimal/=10;
	}
if(precision)
	*--buf='.';
pos+=PrintString(dst, size, buf, pos);
if(exponent!=0||FlagHelper::Get(flags, FormatFlags::Numeric))
	{
	pos+=PrintChar(dst, size, 'e', pos);
	if(exponent<0)
		{
		pos+=PrintChar(dst, size, '-', pos);
		exponent*=-1;
		}
	pos+=PrintUInt(dst, size, (UINT)exponent, pos);
	}
if(pos<size)
	dst[pos]=0;
return pos-start;
}

template <std::character _dst_t, std::unsigned_integral _uint_t> UINT StringHelper::PrintHex(_dst_t* dst, UINT size, _uint_t value, UINT pos)
{
CHAR chars[20];
LPSTR buf=&chars[19];
*buf=0;
do
	{
	CHAR c=(CHAR)((value%16)+'0');
	if(c>'9')
		c=(CHAR)(c+7);
	*--buf=c;
	value/=16;
	}
while(value);
return PrintString(dst, size, buf, pos);
}

template <std::character _dst_t, std::unsigned_integral _uint_t> UINT StringHelper::PrintHex(_dst_t* dst, UINT size, _uint_t value, FormatFlags flags, UINT width, UINT pos)
{
UINT len=0;
if(width>0)
	len=PrintHex((LPSTR)nullptr, 0, value, flags, 0, 0);
UINT start=pos;
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&!FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=PrintChars(dst, size, ' ', width-len, pos);
	}
if(FlagHelper::Get(flags, FormatFlags::Numeric))
	pos+=PrintString(dst, size, "0x", pos);
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=PrintChars(dst, size, '0', width-len, pos);
	}
pos+=PrintHex(dst, size, value, pos);
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		PrintChars(dst, size, ' ', width-len, pos);
	}
if(pos<size)
	dst[pos]=0;
return pos-start;
}

template <std::character _dst_t, std::signed_integral _int_t> UINT StringHelper::PrintInt(_dst_t* dst, UINT size, _int_t value, FormatFlags flags, UINT width, UINT pos)
{
UINT len=0;
if(width>0)
	len=PrintInt((LPSTR)nullptr, 0, value, flags, 0, 0);
UINT start=pos;
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&!FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=PrintChars(dst, size, ' ', width-len, pos);
	}
if(value<0)
	{
	pos+=PrintChar(dst, size, '-', pos);
	value*=-1;
	}
else if(FlagHelper::Get(flags, FormatFlags::Signed))
	{
	pos+=PrintChar(dst, size, '+', pos);
	}
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=PrintChars(dst, size, '0', width-len, pos);
	}
using _uint_t=std::make_unsigned_t<_int_t>;
pos+=PrintUInt(dst, size, (_uint_t)value, pos);
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		pos+=PrintChars(dst, size, ' ', width-len, pos);
	}
if(pos<size)
	dst[pos]=0;
return pos-start;
}

template <std::character _dst_t, std::character _src_t> UINT StringHelper::PrintString(_dst_t* dst, UINT size, _src_t const* src, UINT pos)
{
if(!src)
	return 0;
UINT src_pos=0;
for(; src[src_pos]; src_pos++)
	{
	if(pos+1==size)
		break;
	if(dst)
		dst[pos++]=CharHelper::ToChar<_dst_t, _src_t>(src[src_pos]);
	}
return src_pos;
}

template <std::character _dst_t, std::character _src_t> UINT StringHelper::PrintString(_dst_t* dst, UINT size, _src_t const* src, FormatFlags flags, UINT width, UINT pos)
{
if(!src)
	return 0;
UINT start=pos;
UINT len=0;
if(width>0)
	len=Length(src);
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left))
		pos+=PrintChars(dst, size, ' ', width-len, pos);
	}
if(FlagHelper::Get(flags, FormatFlags::High))
	{
	pos+=PrintStringCapital(dst, size, src, pos);
	}
else if(FlagHelper::Get(flags, FormatFlags::Low))
	{
	pos+=PrintStringSmall(dst, size, src, pos);
	}
else
	{
	pos+=PrintString(dst, size, src, pos);
	}
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		pos+=PrintChars(dst, size, ' ', width-len, pos);
	}
if(pos<size)
	dst[pos]=0;
return pos-start;
}

template <std::character _dst_t, std::character _src_t> UINT StringHelper::PrintStringCapital(_dst_t* dst, UINT size, _src_t const* src, UINT pos)
{
if(!src)
	return 0;
UINT src_pos=0;
for(; src[src_pos]; src_pos++)
	{
	if(pos+1==size)
		break;
	if(dst)
		dst[pos++]=CharHelper::ToCapital<_dst_t, _src_t>(src[src_pos]);
	}
return src_pos;
}

template <std::character _dst_t, std::character _src_t> UINT StringHelper::PrintStringSmall(_dst_t* dst, UINT size, _src_t const* src, UINT pos)
{
if(!src)
	return 0;
UINT src_pos=0;
for(; src[src_pos]; src_pos++)
	{
	if(pos+1==size)
		break;
	if(dst)
		dst[pos++]=CharHelper::ToSmall<_dst_t, _src_t>(src[src_pos]);
	}
return src_pos;
}

template <std::character _dst_t, std::unsigned_integral _uint_t> UINT StringHelper::PrintUInt(_dst_t* dst, UINT size, _uint_t value, UINT pos)
{
CHAR chars[22];
LPSTR buf=&chars[21];
*buf=0;
do
	{
	*--buf=(CHAR)((value%10)+'0');
	value/=10;
	}
while(value);
return PrintString(dst, size, buf, pos);
}

template <std::character _dst_t, std::unsigned_integral _uint_t> UINT StringHelper::PrintUInt(_dst_t* dst, UINT size, _uint_t value, FormatFlags flags, UINT width, UINT pos)
{
UINT len=0;
if(width>0)
	len=PrintUInt((LPSTR)nullptr, 0, value, flags, 0, 0);
UINT start=pos;
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&!FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=PrintChars(dst, size, ' ', width-len, pos);
	}
if(FlagHelper::Get(flags, FormatFlags::Signed))
	pos+=PrintChar(dst, size, '+', pos);
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=PrintChars(dst, size, '0', width-len, pos);
	}
pos+=PrintUInt(dst, size, value, pos);
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		pos+=PrintChars(dst, size, ' ', width-len, pos);
	}
if(pos<size)
	dst[pos]=0;
return pos-start;
}

template <std::character _dst_t, std::character _src_t, std::character _find_t, std::character _insert_t> UINT StringHelper::Replace(_dst_t* dst, UINT size, _src_t const* str, _find_t const* find, _insert_t const* insert, BOOL cs, BOOL repeat)
{
if(!str)
	return 0;
UINT find_len=Length(find);
if(find_len==0)
	return Copy(dst, size, str);
BOOL parse=true;
UINT pos=0;
while(*str)
	{
	if(pos==size-1)
		{
		if(dst)
			*dst=0;
		return pos;
		}
	if(parse)
		{
		if(Compare(str, find, find_len, cs)==0)
			{
			UINT insert_len=Copy(dst, size, insert);
			if(dst)
				{
				dst+=insert_len;
				size-=insert_len;
				}
			pos+=insert_len;
			if(!repeat)
				parse=false;
			str+=find_len;
			continue;
			}
		}
	if(dst)
		{
		*dst++=*str;
		}
	str++;
	pos++;
	}
if(dst)
	*dst=0;
return pos;
}

template <std::character _str_t> UINT StringHelper::Scan(_str_t const* str, LPCSTR format, VariableArguments& args)
{
if(!str||!format)
	return 0;
args.Reset();
UINT count=0;
UINT pos=0;
for(UINT fmt=0; format[fmt]; )
	{
	if(str[pos]==0)
		return count;
	Format str_format=Format::None;
	FormatFlags flags=FormatFlags::None;
	UINT width=UINT_MAX;
	UINT prec=UINT_MAX;
	fmt+=GetFormat(&format[fmt], str_format, flags, width, prec);
	if(str_format==Format::None)
		{
		if(str[pos++]!=format[fmt++])
			return count;
		continue;
		}
	if(str_format==Format::Percent)
		{
		if(!CharHelper::Equal(str[pos++], '%'))
			return count;
		continue;
		}
	UINT base=10;
	switch(str_format)
		{
		case Format::Char:
			{
			_str_t tc=str[pos];
			if(args.Set(tc))
				{
				count++;
				pos++;
				continue;
				}
			return count;
			}
		case Format::Int:
			{
			INT64 i=0;
			UINT len=ScanInt(&str[pos], &i);
			if(!len)
				return count;
			if(args.Set(i))
				{
				count++;
				pos+=len;
				continue;
				}
			return count;
			}
		case Format::Hex:
			base=16;
			[[fallthrough]];
		case Format::UInt:
			{
			UINT64 u=0;
			UINT len=ScanUInt(&str[pos], &u, base);
			if(!len)
				return count;
			if(args.Set(u))
				{
				count++;
				pos+=len;
				continue;
				}
			return count;
			}
		case Format::Float:
			{
			FLOAT f=0;
			UINT len=ScanFloat(&str[pos], &f);
			if(!len)
				return count;
			if(args.Set(f))
				{
				count++;
				pos+=len;
				continue;
				}
			return count;
			}
		case Format::Double:
			{
			DOUBLE d=0;
			UINT len=ScanFloat(&str[pos], &d);
			if(!len)
				return count;
			if(args.Set(d))
				{
				count++;
				pos+=len;
				continue;
				}
			return count;
			}
		case Format::String:
			{
			UINT len=ScanString(&str[pos], (LPTSTR)nullptr, 0, format[fmt]);
			if(args.Set(&str[pos], len))
				{
				count++;
				pos+=len;
				continue;
				}
			return count;
			}
		default:
			break;
		}
	}
return count;
}

template <std::character _str_t, std::floating_point _float_t> UINT StringHelper::ScanFloat(_str_t const* str, _float_t* value_ptr)
{
if(!str)
	return 0;
UINT pos=0;
BOOL negative=false;
for(; CharHelper::Equal(str[pos], '-'); pos++)
	{
	negative=!negative;
	}
if(!CharHelper::IsDigit(str[pos]))
	return 0;
_float_t f=(_float_t)str[pos]-'0';
for(pos++; str[pos]; pos++)
	{
	if(!CharHelper::IsDigit(str[pos]))
		break;
	f*=10;
	f+=(_float_t)str[pos]-'0';
	}
if(str[pos])
	{
	if(CharHelper::Equal(str[pos], '.')||CharHelper::Equal(str[pos], ','))
		{
		_float_t div=10;
		for(pos++; str[pos]; pos++)
			{
			if(!CharHelper::IsDigit(str[pos]))
				break;
			f+=((_float_t)str[pos]-'0')/div;
			div*=10;
			}
		}
	}
if(str[pos])
	{
	if(CharHelper::Equal(str[pos], 'E')||CharHelper::Equal(str[pos], 'e'))
		{
		pos++;
		INT ex=0;
		UINT ex_len=ScanInt(&str[pos], &ex);
		if(ex_len==0)
			return 0;
		pos+=ex_len;
		ex*=10;
		if(ex<0)
			{
			f/=(_float_t)-ex;
			}
		else
			{
			f*=(_float_t)ex;
			}
		}
	}
if(negative)
	f*=-1;
if(value_ptr)
	*value_ptr=f;
return pos;
}

template <std::character _str_t, std::signed_integral _int_t> UINT StringHelper::ScanInt(_str_t const* str, _int_t* value_ptr)
{
if(value_ptr)
	*value_ptr=0;
if(!str)
	return 0;
UINT pos=0;
BOOL negative=false;
for(; CharHelper::Equal(str[pos], '-'); pos++)
	{
	negative=!negative;
	}
UINT digit=0;
if(!CharHelper::ToDigit(str[pos], &digit))
	return 0;
_int_t i=digit;
for(pos++; str[pos]; pos++)
	{
	if(!CharHelper::ToDigit(str[pos], &digit))
		break;
	i*=10;
	i+=digit;
	}
if(negative)
	i*=-1;
if(value_ptr)
	*value_ptr=i;
return pos;
}

template <std::character _str_t, std::unsigned_integral _uint_t> UINT StringHelper::ScanUInt(_str_t const* str, _uint_t* value_ptr, UINT base, UINT len)
{
if(!str)
	return 0;
if(value_ptr)
	*value_ptr=0;
UINT pos=0;
while(CharHelper::Equal(str[pos], ' '))
	{
	if(++pos==len)
		return pos;
	}
if(CharHelper::Equal(str[pos], '0'))
	{
	pos++;
	if(pos==len)
		return pos;
	if(CharHelper::Equal(str[pos], 'b'))
		{
		pos++;
		if(base!=2)
			return pos;
		}
	else if(CharHelper::Equal(str[pos], 'x'))
		{
		pos++;
		if(base!=16)
			return pos;
		}
	}
UINT value=0;
for(; str[pos]; pos++)
	{
	UINT digit=0;
	if(!CharHelper::ToDigit(str[pos], &digit, base))
		break;
	value*=base;
	value+=digit;
	}
if(value_ptr)
	*value_ptr=value;
return pos;
}

template <std::character _str_t, std::character _buf_t> UINT StringHelper::ScanString(_str_t const* str, _buf_t* buf, UINT size, CHAR esc)
{
if(!str)
	return 0;
UINT pos=0;
for(; str[pos]; pos++)
	{
	if(CharHelper::Equal(str[pos], esc))
		break;
	if(pos<size)
		buf[pos]=CharHelper::ToChar<_buf_t, _str_t>(str[pos]);
	}
if(pos<size)
	buf[pos]=0;
return pos;
}

template <std::character _str_t> _str_t const* StringHelper::Truncate(_str_t const* str, LPCSTR chars)
{
UINT len=Length(chars);
while(*str)
	{
	bool trunc=false;
	for(UINT u=0; u<len; u++)
		{
		if(CharHelper::Compare(*str, chars[u])==0)
			{
			trunc=true;
			break;
			}
		}
	if(!trunc)
		break;
	str++;
	}
return str;
}

template <std::character _str_t> UINT StringHelper::Uppercase(_str_t* str)
{
if(!str)
	return 0;
UINT pos=0;
for(; str[pos]; pos++)
	str[pos]=CharHelper::ToCapital<_str_t, _str_t>(str[pos]);
return pos;
}

template <std::character _dst_t, std::character _src_t> UINT StringHelper::Uppercase(_dst_t* dst, UINT size, _src_t const* str)
{
if(!str)
	return 0;
UINT pos=0;
for(; str[pos]; pos++)
	{
	if(pos+1==size)
		break;
	if(dst)
		dst[pos]=CharHelper::ToCapital<_dst_t, _src_t>(str[pos]);
	}
if(dst)
	dst[pos]=0;
return pos;
}
