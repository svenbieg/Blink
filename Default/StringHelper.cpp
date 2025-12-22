//==================
// StringHelper.cpp
//==================

#include "StringHelper.h"


//=======
// Using
//=======

#include <cmath>
#include "Storage/Streams/StreamWriter.h"
#include "CharHelper.h"
#include "FlagHelper.h"
#include "StringClass.h"

using namespace Storage::Streams;

using Format=StringHelper::Format;
using FormatFlags=StringHelper::FormatFlags;


//======================
// Forward-Declarations
//======================

template <class _dst_t, class _src_t> inline _dst_t CharToCapital(_src_t c) {}

template <> inline CHAR CharToCapital(CHAR c)
{
return CharHelper::ToCapitalAnsi(c);
}

template <> inline CHAR CharToCapital(WCHAR c)
{
return CharHelper::ToCapitalAnsi(c);
}

template <> inline WCHAR CharToCapital(CHAR c)
{
return CharHelper::ToCapitalUnicode(c);
}

template <> inline WCHAR CharToCapital(WCHAR c)
{
return CharHelper::ToCapitalUnicode(c);
}

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

template <class _dst_t, class _src_t> inline _dst_t CharToSmall(_src_t c) {}

template <> inline CHAR CharToSmall(CHAR c)
{
return CharHelper::ToSmallAnsi(c);
}

template <> inline CHAR CharToSmall(WCHAR c)
{
return CharHelper::ToSmallAnsi(c);
}

template <> inline WCHAR CharToSmall(CHAR c)
{
return CharHelper::ToSmallUnicode(c);
}

template <> inline WCHAR CharToSmall(WCHAR c)
{
return CharHelper::ToSmallUnicode(c);
}

template <class _char_t> inline UINT StringLength(_char_t const* value)
{
if(!value)
	return 0;
UINT len=0;
for(; value[len]; len++);
return len;
}

template <class _char_t> inline UINT StringLength(_char_t const* value, UINT max)
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


//========
// Common
//========

template <class _dst_t, class _src_t> inline UINT StringCopy(_dst_t* dst, UINT size, _src_t const* src, UINT copy)
{
if(!src)
	{
	if(dst)
		dst[0]=0;
	return 0;
	}
if(size==0)
	size=UINT_MAX;
UINT end=size-1;
if(copy)
	end=TypeHelper::Min(end, copy);
UINT pos=0;
for(; pos<end; pos++)
	{
	if(src[pos]==0)
		break;
	if(dst)
		dst[pos]=CharToChar<_dst_t, _src_t>(src[pos]);
	}
if(dst)
	dst[pos]=0;
return pos;
}

template <class _char_t, class _find_t> inline BOOL StringFind(_char_t const* str, _find_t const* find, UINT* pos_ptr, BOOL cs)
{
if(!str||!find)
	return false;
UINT find_len=StringLength(find);
if(!find_len)
	return false;
for(UINT pos=0; str[pos]; pos++)
	{
	if(StringHelper::Compare(&str[pos], find, find_len, cs)==0)
		{
		if(pos_ptr)
			*pos_ptr=pos;
		return true;
		}
	}
return false;
}

template <class _str_t, class _char_t> inline BOOL StringFindChar(_str_t const* str, _char_t c, UINT* pos_ptr, BOOL cs)
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

template <class _char_t, class _find_t> inline BOOL StringFindChars(_char_t const* str, _find_t const* find, UINT* pos_ptr, BOOL cs)
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

template <class _char_t> inline UINT StringLowercase(_char_t* str)
{
if(!str)
	return 0;
UINT pos=0;
for(; str[pos]; pos++)
	str[pos]=CharToSmall<_char_t, _char_t>(str[pos]);
return pos;
}

template <class _dst_t, class _char_t, class _find_t, class _insert_t> inline UINT StringReplace(_dst_t* dst, UINT size, _char_t const* str, _find_t const* find, _insert_t const* insert, BOOL cs, BOOL repeat)
{
if(!str)
	return 0;
UINT find_len=StringHelper::Length(find);
if(find_len==0)
	return StringHelper::Copy(dst, size, str);
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
		if(StringHelper::Compare(str, find, find_len, cs)==0)
			{
			UINT insert_len=StringHelper::Copy(dst, size, insert);
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

template <class _char_t> inline _char_t const* StringTruncate(_char_t const* str, LPCSTR chars)
{
UINT len=StringHelper::Length(chars);
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

template <class _char_t> inline UINT StringUppercase(_char_t* str)
{
if(!str)
	return 0;
UINT pos=0;
for(; str[pos]; pos++)
	str[pos]=CharToCapital<_char_t, _char_t>(str[pos]);
return pos;
}


//==========
// Scanning
//==========

template <class _char_t, class _int_t> UINT StringScanInt(_char_t const* str, _int_t* value_ptr)
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

template <class _char_t, class _uint_t> UINT StringScanUInt(_char_t const* str, _uint_t* value_ptr, UINT base, UINT len)
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

template <class _char_t, class _float_t> UINT StringScanFloat(_char_t const* str, _float_t* value_ptr)
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
		UINT ex_len=StringScanInt(&str[pos], &ex);
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

template <class _char_t, class _buf_t, class _stop_t> UINT StringScanString(_char_t const* str, _buf_t* buf, UINT size, _stop_t stop)
{
if(!str)
	return 0;
UINT pos=0;
for(; str[pos]; pos++)
	{
	if(CharHelper::Equal(str[pos], stop))
		break;
	if(pos<size)
		buf[pos]=CharToChar<_buf_t, _char_t>(str[pos]);
	}
if(pos<size)
	buf[pos]=0;
return pos;
}


//==========
// Printing
//==========

template <class _buf_t, class _char_t> UINT StringPrintChar(_buf_t* buf, UINT size, _char_t c, UINT pos)
{
if(!buf)
	return 1;
if(pos>=size)
	return 0;
buf[pos]=CharToChar<_buf_t, _char_t>(c);
return 1;
}

template <class _buf_t, class _char_t> UINT StringPrintChar(_buf_t* buf, UINT size, _char_t c, FormatFlags flags, UINT pos)
{
if(!buf)
	return 1;
if(pos>=size)
	return 0;
if(FlagHelper::Get(flags, FormatFlags::High))
	{
	buf[pos]=CharToCapital<_buf_t, _char_t>(c);
	}
else if(FlagHelper::Get(flags, FormatFlags::Low))
	{
	buf[pos]=CharToSmall<_buf_t, _char_t>(c);
	}
else
	{
	buf[pos]=CharToChar<_buf_t, _char_t>(c);
	}
return 1;
}

template <class _buf_t, class _char_t> UINT StringPrintChars(_buf_t* buf, UINT size, _char_t c, UINT count, UINT pos)
{
if(!buf)
	return count;
if(pos>=size)
	return 0;
UINT print=TypeHelper::Min(size-pos, count);
for(UINT u=0; u<print; u++)
	buf[pos++]=CharToChar<_buf_t, _char_t>(c);
return print;
}

template <class _buf_t, class _char_t> UINT StringPrintString(_buf_t* buf, UINT size, _char_t const* value, UINT pos)
{
if(!value)
	return 0;
UINT value_pos=0;
for(; value[value_pos]; value_pos++)
	{
	if(pos+1==size)
		break;
	if(buf)
		buf[pos++]=CharToChar<_buf_t, _char_t>(value[value_pos]);
	}
return value_pos;
}

template <class _buf_t, class _char_t> UINT StringPrintStringCapital(_buf_t* buf, UINT size, _char_t const* value, UINT pos)
{
if(!value)
	return 0;
UINT value_pos=0;
for(; value[value_pos]; value_pos++)
	{
	if(pos+1==size)
		break;
	if(buf)
		buf[pos++]=CharToCapital<_buf_t, _char_t>(value[value_pos]);
	}
return value_pos;
}

template <class _buf_t, class _char_t> UINT StringPrintStringSmall(_buf_t* buf, UINT size, _char_t const* value, UINT pos)
{
if(!value)
	return 0;
UINT value_pos=0;
for(; value[value_pos]; value_pos++)
	{
	if(pos+1==size)
		break;
	if(buf)
		buf[pos++]=CharToSmall<_buf_t, _char_t>(value[value_pos]);
	}
return value_pos;
}

template <class _buf_t, class _char_t> UINT StringPrintString(_buf_t* buf, UINT size, _char_t const* value, FormatFlags flags, UINT width, UINT pos)
{
if(!value)
	return 0;
UINT start=pos;
UINT len=0;
if(width>0)
	len=StringHelper::Length(value);
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left))
		pos+=StringPrintChars(buf, size, ' ', width-len, pos);
	}
if(FlagHelper::Get(flags, FormatFlags::High))
	{
	pos+=StringPrintStringCapital(buf, size, value, pos);
	}
else if(FlagHelper::Get(flags, FormatFlags::Low))
	{
	pos+=StringPrintStringSmall(buf, size, value, pos);
	}
else
	{
	pos+=StringPrintString(buf, size, value, pos);
	}
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		pos+=StringPrintChars(buf, size, ' ', width-len, pos);
	}
if(pos<size)
	buf[pos]=0;
return pos-start;
}


//===================
// Printing Integers
//===================

template <class _char_t, class _uint_t> UINT StringPrintUInt(_char_t* str, UINT size, _uint_t value, UINT pos=0)
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
return StringPrintString(str, size, buf, pos);
}

template <class _char_t, class _uint_t> UINT StringPrintUInt(_char_t* str, UINT size, _uint_t value, FormatFlags flags, UINT width, UINT pos=0)
{
UINT len=0;
if(width>0)
	len=StringPrintUInt((CHAR*)nullptr, 0, value, flags, 0);
UINT start=pos;
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&!FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(str, size, ' ', width-len, pos);
	}
if(FlagHelper::Get(flags, FormatFlags::Signed))
	pos+=StringPrintChar(str, size, '+', pos);
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(str, size, '0', width-len, pos);
	}
pos+=StringPrintUInt(str, size, value, pos);
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		pos+=StringPrintChars(str, size, ' ', width-len, pos);
	}
if(pos<size)
	str[pos]=0;
return pos-start;
}

template <class _char_t, class _int_t> UINT StringPrintInt(_char_t* str, UINT size, _int_t value, FormatFlags flags, UINT width, UINT pos=0)
{
UINT len=0;
if(width>0)
	len=StringPrintInt((CHAR*)nullptr, 0, value, flags, 0);
UINT start=pos;
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&!FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(str, size, ' ', width-len, pos);
	}
if(value<0)
	{
	pos+=StringPrintChar(str, size, '-', pos);
	value*=-1;
	}
else if(FlagHelper::Get(flags, FormatFlags::Signed))
	{
	pos+=StringPrintChar(str, size, '+', pos);
	}
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(str, size, '0', width-len, pos);
	}
pos+=StringPrintUInt(str, size, value, pos);
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		pos+=StringPrintChars(str, size, ' ', width-len, pos);
	}
if(pos<size)
	str[pos]=0;
return pos-start;
}

template <class _char_t, class _uint_t> UINT StringPrintHex(_char_t* str, UINT size, _uint_t value, UINT pos=0)
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
return StringPrintString(str, size, buf, pos);
}

template <class _char_t, class _uint_t> UINT StringPrintHex(_char_t* str, UINT size, _uint_t value, FormatFlags flags, UINT width, UINT pos=0)
{
UINT len=0;
if(width>0)
	len=StringPrintHex((CHAR*)nullptr, 0, value, flags, 0);
UINT start=pos;
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&!FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(str, size, ' ', width-len, pos);
	}
if(FlagHelper::Get(flags, FormatFlags::Numeric))
	pos+=StringPrintString(str, size, "0x", pos);
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(str, size, '0', width-len, pos);
	}
pos+=StringPrintHex(str, size, value, pos);
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		StringPrintChars(str, size, ' ', width-len, pos);
	}
if(pos<size)
	str[pos]=0;
return pos-start;
}


//=================
// Printing Floats
//=================

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

template <class _char_t, class _float_t> UINT StringPrintFloat(_char_t* str, UINT size, _float_t f, FormatFlags flags, UINT width, UINT precision, UINT pos=0)
{
UINT start=pos;
if(std::isnan(f))
	{
	pos+=StringPrintString(str, size, "nan", pos);
	return pos-start;
	}
if(f<0)
	{
	pos+=StringPrintChar(str, size, '-', pos);
	f*=-1;
	}
else if(FlagHelper::Get(flags, FormatFlags::Signed))
	{
	pos+=StringPrintChar(str, size, '+', pos);
	}
if(std::isinf(f))
	{
	pos+=StringPrintString(str, size, "inf", pos);
	return pos-start;
	}
UINT integral=0;
UINT decimal=0;
INT exponent=0;
FloatSplit(f, integral, decimal, exponent, width, precision);
pos+=StringPrintUInt(str, size, integral, pos);
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
pos+=StringPrintString(str, size, buf, pos);
if(exponent!=0||FlagHelper::Get(flags, FormatFlags::Numeric))
	{
	pos+=StringPrintChar(str, size, 'e', pos);
	if(exponent<0)
		{
		pos+=StringPrintChar(str, size, '-', pos);
		exponent*=-1;
		}
	pos+=StringPrintUInt(str, size, exponent, pos);
	}
if(pos<size)
	str[pos]=0;
return pos-start;
}


//============
// Formatting
//============

template <class _char_t> inline UINT StringPrintArgs(_char_t* str, UINT size, LPCSTR format, VariableArguments const& args)
{
if(!format)
	return 0;
UINT pos=0;
UINT arg=0;
for(UINT fmt=0; format[fmt]; )
	{
	if(pos+1==size)
		break;
	Format str_format=Format::None;
	FormatFlags flags=FormatFlags::None;
	UINT width=0;
	UINT prec=UINT_MAX;
	fmt+=StringHelper::GetFormat(&format[fmt], str_format, flags, width, prec);
	if(str_format==Format::None)
		{
		pos+=StringPrintChar(str, size, format[fmt++], pos);
		continue;
		}
	if(str_format==Format::Percent)
		{
		pos+=StringPrintChar(str, size, '%', pos);
		continue;
		}
	if(FlagHelper::Get(flags, FormatFlags::Width))
		{
		if(!args.GetAt(arg++, width))
			return 0;
		}
	if(FlagHelper::Get(flags, FormatFlags::Precision))
		{
		if(!args.GetAt(arg++, prec))
			return 0;
		}
	switch(str_format)
		{
		case Format::Int:
			{
			INT64 i=0;
			if(!args.GetAt(arg++, i))
				return 0;
			pos+=StringPrintInt(str, size, i, flags, width, pos);
			continue;
			}
		case Format::UInt:
			{
			UINT64 u=0;
			if(!args.GetAt(arg++, u))
				return 0;
			pos+=StringPrintUInt(str, size, u, flags, width, pos);
			continue;
			}
		case Format::Hex:
			{
			UINT64 u=0;
			if(!args.GetAt(arg++, u))
				return 0;
			pos+=StringPrintHex(str, size, u, flags, width, pos);
			continue;
			}
		case Format::Float:
			{
			FLOAT f=0;
			if(!args.GetAt(arg++, f))
				return 0;
			pos+=StringPrintFloat(str, size, f, flags, width, prec, pos);
			continue;
			}
		case Format::Double:
			{
			DOUBLE d=0;
			if(!args.GetAt(arg++, d))
				return 0;
			pos+=StringPrintFloat(str, size, d, flags, width, prec, pos);
			continue;
			}
		case Format::Char:
			{
			WCHAR c=' ';
			if(!args.GetAt(arg++, c))
				return 0;
			pos+=StringPrintChar(str, size, c, flags, pos);
			continue;
			}
		case Format::String:
			{
			LPCSTR p=nullptr;
			if(args.GetAt(arg, p))
				{
				arg++;
				pos+=StringPrintString(str, size, p, flags, width, pos);
				continue;
				}
			LPCWSTR pw=nullptr;
			if(args.GetAt(arg, pw))
				{
				arg++;
				pos+=StringPrintString(str, size, pw, flags, width, pos);
				continue;
				}
			return 0;
			}
		default:
			continue;
		}
	}
if(pos<size)
	str[pos]=0;
return pos;
}


//==========
// Scanning
//==========

template <class _char_t> inline UINT StringScanArgs(_char_t const* str, LPCSTR format, VariableArguments& args)
{
if(!str||!format)
	return 0;
UINT pos=0;
UINT arg=0;
UINT read=0;
for(UINT fmt=0; format[fmt]; )
	{
	if(str[pos]==0)
		return read;
	Format str_format=Format::None;
	FormatFlags flags=FormatFlags::None;
	UINT width=UINT_MAX;
	UINT prec=UINT_MAX;
	fmt+=StringHelper::GetFormat(&format[fmt], str_format, flags, width, prec);
	if(str_format==Format::None)
		{
		if(str[pos++]!=format[fmt++])
			return read;
		continue;
		}
	if(str_format==Format::Percent)
		{
		if(!CharHelper::Equal(str[pos++], '%'))
			return read;
		continue;
		}
	UINT base=10;
	switch(str_format)
		{
		case Format::Char:
			{
			_char_t tc=str[pos];
			CHAR* pc=nullptr;
			if(args.GetAt(arg, pc))
				{
				*pc=CharHelper::ToAnsi(tc);
				arg++;
				read++;
				pos++;
				continue;
				}
			WCHAR* pwc=nullptr;
			if(args.GetAt(arg, pwc))
				{
				*pwc=CharHelper::ToUnicode(tc);
				arg++;
				read++;
				pos++;
				continue;
				}
			return read;
			}
		case Format::Int:
			{
			INT64 i=0;
			UINT len=StringHelper::ScanInt(&str[pos], &i);
			if(!len)
				return read;
			INT* p32=nullptr;
			if(args.GetAt(arg, p32))
				{
				if(i<INT_MIN||i>INT_MAX)
					return read;
				*p32=(INT)i;
				arg++;
				read++;
				pos+=len;
				continue;
				}
			INT64* p64=nullptr;
			if(args.GetAt(arg, p64))
				{
				*p64=i;
				arg++;
				read++;
				pos+=len;
				continue;
				}
			return read;
			}
		case Format::Hex:
			base=16;
			[[fallthrough]];
		case Format::UInt:
			{
			UINT64 u=0;
			UINT len=StringHelper::ScanUInt(&str[pos], &u, base);
			if(!len)
				return read;
			UINT* p32=nullptr;
			if(args.GetAt(arg, p32))
				{
				if(u>UINT_MAX)
					return read;
				*p32=(UINT)u;
				arg++;
				read++;
				pos+=len;
				continue;
				}
			UINT64* p64=nullptr;
			if(args.GetAt(arg, p64))
				{
				*p64=u;
				arg++;
				read++;
				pos+=len;
				continue;
				}
			return read;
			}
		case Format::Float:
			{
			FLOAT f=0;
			UINT len=StringHelper::ScanFloat(&str[pos], &f);
			if(!len)
				return read;
			FLOAT* p=nullptr;
			if(args.GetAt(arg, p))
				{
				*p=f;
				arg++;
				read++;
				pos+=len;
				continue;
				}
			return read;
			}
		case Format::Double:
			{
			DOUBLE d=0;
			UINT len=StringHelper::ScanFloat(&str[pos], &d);
			if(!len)
				return read;
			DOUBLE* p=nullptr;
			if(args.GetAt(arg, p))
				{
				*p=d;
				arg++;
				read++;
				pos+=len;
				continue;
				}
			return read;
			}
		case Format::String:
			{
			LPSTR p=nullptr;
			LPWSTR pw=nullptr;
			if(!args.GetAt(arg, p))
				{
				if(!args.GetAt(arg, pw))
					return read;
				}
			UINT size=0;
			if(!args.GetAt(arg+1, size))
				return read;
			UINT len=0;
			if(p)
				{
				len=StringScanString(&str[pos], p, size, format[fmt]);
				}
			else if(pw)
				{
				len=StringScanString(&str[pos], pw, size, format[fmt]);
				}
			if(!len)
				return read;
			arg+=2;
			read++;
			pos+=len;
			continue;
			}
		default:
			break;
		}
	}
return read;
}


//============
// Comparison
//============

template <class _char1_t, class _char2_t> inline INT StringCompare(_char1_t const* str1, _char2_t const* str2, UINT count, BOOL cs)
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
	UINT len1=StringHelper::ScanFloat(&str1[pos1], &f1);
	FLOAT f2=0;
	UINT len2=StringHelper::ScanFloat(&str2[pos2], &f2);
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

template <class _char_t> inline UINT64 StringHash(_char_t const* str)
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


//===============
// String-Helper
//===============

UINT StringHelper::Append(LPSTR dst, UINT size, LPCSTR str, LPCSTR append)
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

INT StringHelper::Compare(LPCSTR str1, LPCSTR str2, UINT len, BOOL cs)
{
return StringCompare(str1, str2, len, cs);
}

INT StringHelper::Compare(LPCSTR str1, LPCWSTR str2, UINT len, BOOL cs)
{
return StringCompare(str1, str2, len, cs);
}

INT StringHelper::Compare(LPCSTR str1, String const* str2, UINT len, BOOL cs)
{
return StringCompare(str1, str2? str2->Begin(): nullptr, len, cs);
}

INT StringHelper::Compare(LPCWSTR str1, LPCSTR str2, UINT len, BOOL cs)
{
return StringCompare(str1, str2, len, cs);
}

INT StringHelper::Compare(LPCWSTR str1, LPCWSTR str2, UINT len, BOOL cs)
{
return StringCompare(str1, str2, len, cs);
}

INT StringHelper::Compare(LPCWSTR str1, String const* str2, UINT len, BOOL cs)
{
return StringCompare(str1, str2? str2->Begin(): nullptr, len, cs);
}

INT StringHelper::Compare(String const* str1, LPCSTR str2, UINT len, BOOL cs)
{
return StringCompare(str1? str1->Begin(): nullptr, str2, len, cs);
}

INT StringHelper::Compare(String const* str1, LPCWSTR str2, UINT len, BOOL cs)
{
return StringCompare(str1? str1->Begin(): nullptr, str2, len, cs);
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
return StringCompare(str1->Begin(), str2->Begin(), 0, false);
}

INT StringHelper::Compare(String const* str1, String const* str2, UINT len, BOOL cs)
{
return StringCompare(str1? str1->Begin(): nullptr, str2? str2->Begin(): nullptr, len, cs);
}

UINT StringHelper::Copy(LPSTR dst, UINT size, LPCSTR src, UINT copy)
{
return StringCopy(dst, size, src, copy);
}

UINT StringHelper::Copy(LPSTR dst, UINT size, LPCWSTR src, UINT copy)
{
return StringCopy(dst, size, src, copy);
}

UINT StringHelper::Copy(LPSTR dst, UINT size, String const* src, UINT copy)
{
if(!src)
	return 0;
return StringCopy(dst, size, src->Begin(), copy);
}

UINT StringHelper::Copy(LPWSTR dst, UINT size, LPCSTR src, UINT copy)
{
return StringCopy(dst, size, src, copy);
}

UINT StringHelper::Copy(LPWSTR dst, UINT size, LPCWSTR src, UINT copy)
{
return StringCopy(dst, size, src, copy);
}

UINT StringHelper::Copy(LPWSTR dst, UINT size, String const* src, UINT copy)
{
if(!src)
	return 0;
return StringCopy(dst, size, src->Begin(), copy);
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
	UINT len=StringLength(src);
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

BOOL StringHelper::Find(LPCSTR str, LPCSTR find, UINT* pos_ptr, BOOL cs)
{
return StringFind(str, find, pos_ptr, cs);
}

BOOL StringHelper::Find(LPCSTR str, LPCWSTR find, UINT* pos_ptr, BOOL cs)
{
return StringFind(str, find, pos_ptr, cs);
}

BOOL StringHelper::Find(LPCWSTR str, LPCSTR find, UINT* pos_ptr, BOOL cs)
{
return StringFind(str, find, pos_ptr, cs);
}

BOOL StringHelper::Find(LPCWSTR str, LPCWSTR find, UINT* pos_ptr, BOOL cs)
{
return StringFind(str, find, pos_ptr, cs);
}

BOOL StringHelper::FindChar(LPCSTR str, CHAR c, UINT* pos_ptr, BOOL cs)
{
return StringFindChar(str, c, pos_ptr, cs);
}

BOOL StringHelper::FindChar(LPCWSTR str, CHAR c, UINT* pos_ptr, BOOL cs)
{
return StringFindChar(str, c, pos_ptr, cs);
}

BOOL StringHelper::FindChars(LPCSTR str, LPCSTR find, UINT* pos_ptr, BOOL cs)
{
return StringFindChars(str, find, pos_ptr, cs);
}

BOOL StringHelper::FindChars(LPCWSTR str, LPCSTR find, UINT* pos_ptr, BOOL cs)
{
return StringFindChars(str, find, pos_ptr, cs);
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
// Size
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
CHAR type=str[pos];
if(type==0)
	{
	format=Format::None;
	return pos;
	}
switch(type)
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

UINT64 StringHelper::GetHash(LPCSTR str)
{
return StringHash(str);
}

UINT64 StringHelper::GetHash(LPCWSTR str)
{
return StringHash(str);
}

UINT StringHelper::Insert(LPSTR dst, UINT size, LPCSTR str, UINT pos, LPCSTR insert)
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

UINT StringHelper::Length(LPCSTR str)
{
return StringLength(str);
}

UINT StringHelper::Length(LPCWSTR str)
{
return StringLength(str);
}

UINT StringHelper::Length(LPCSTR str, UINT max)
{
return StringLength(str, max);
}

UINT StringHelper::Length(LPCWSTR str, UINT max)
{
return StringLength(str, max);
}

UINT StringHelper::Length(LPCSTR format, VariableArguments const& args)
{
return PrintArgs((LPSTR)nullptr, 0, format, args);
}

UINT StringHelper::Lowercase(LPSTR str)
{
return StringLowercase(str);
}

UINT StringHelper::Lowercase(LPWSTR str)
{
return StringLowercase(str);
}

UINT StringHelper::Lowercase(LPSTR dst, UINT size, LPCSTR str)
{
if(!str)
	return 0;
UINT pos=0;
for(; str[pos]; pos++)
	{
	if(pos+1==size)
		break;
	if(dst)
		dst[pos]=CharHelper::ToSmallAnsi(str[pos]);
	}
if(dst)
	dst[pos]=0;
return pos;
}

UINT StringHelper::PrintArgs(LPSTR str, UINT size, LPCSTR format, VariableArguments const& args)
{
return StringPrintArgs(str, size, format, args);
}

UINT StringHelper::PrintArgs(LPWSTR str, UINT size, LPCSTR format, VariableArguments const& args)
{
return StringPrintArgs(str, size, format, args);
}

UINT StringHelper::PrintDouble(LPSTR str, UINT size, DOUBLE value, FormatFlags flags, UINT width, UINT prec)
{
return StringPrintFloat(str, size, value, flags, width, prec);
}

UINT StringHelper::PrintFloat(LPSTR str, UINT size, FLOAT value, FormatFlags flags, UINT width, UINT prec)
{
return StringPrintFloat(str, size, value, flags, width, prec);
}

UINT StringHelper::PrintHex(LPSTR str, UINT size, UINT value, FormatFlags flags, UINT width)
{
return StringPrintHex(str, size, value, flags, width);
}

UINT StringHelper::PrintHex(LPSTR str, UINT size, UINT64 value, FormatFlags flags, UINT width)
{
return StringPrintHex(str, size, value, flags, width);
}

UINT StringHelper::PrintInt(LPSTR str, UINT size, INT value, FormatFlags flags, UINT width)
{
return StringPrintInt(str, size, value, flags, width);
}

UINT StringHelper::PrintInt(LPSTR str, UINT size, INT64 value, FormatFlags flags, UINT width)
{
return StringPrintInt(str, size, value, flags, width);
}

UINT StringHelper::PrintUInt(LPSTR str, UINT size, UINT value)
{
return StringPrintUInt(str, size, value);
}

UINT StringHelper::PrintUInt(LPSTR str, UINT size, UINT value, FormatFlags flags, UINT width)
{
return StringPrintUInt(str, size, value, flags, width);
}

UINT StringHelper::PrintUInt(LPSTR str, UINT size, UINT64 value)
{
return StringPrintUInt(str, size, value);
}

UINT StringHelper::PrintUInt(LPSTR str, UINT size, UINT64 value, FormatFlags flags, UINT width)
{
return StringPrintUInt(str, size, value, flags, width);
}

UINT StringHelper::Replace(LPSTR dst, UINT size, LPCSTR str, LPCSTR find, LPCSTR insert, BOOL cs, BOOL repeat)
{
return StringReplace(dst, size, str, find, insert, cs, repeat);
}

UINT StringHelper::Replace(LPWSTR dst, UINT size, LPCWSTR str, LPCSTR find, LPCSTR insert, BOOL cs, BOOL repeat)
{
return StringReplace(dst, size, str, find, insert, cs, repeat);
}

UINT StringHelper::ScanArgs(LPCSTR str, LPCSTR format, VariableArguments& args)
{
return StringScanArgs(str, format, args);
}

UINT StringHelper::ScanArgs(LPCWSTR str, LPCSTR format, VariableArguments& args)
{
return StringScanArgs(str, format, args);
}

UINT StringHelper::ScanFloat(LPCSTR str, FLOAT* value)
{
return StringScanFloat(str, value);
}

UINT StringHelper::ScanFloat(LPCWSTR str, FLOAT* value)
{
return StringScanFloat(str, value);
}

UINT StringHelper::ScanFloat(LPCSTR str, DOUBLE* value)
{
return StringScanFloat(str, value);
}

UINT StringHelper::ScanFloat(LPCWSTR str, DOUBLE* value)
{
return StringScanFloat(str, value);
}

UINT StringHelper::ScanInt(LPCSTR str, INT* value_ptr)
{
return StringScanInt(str, value_ptr);
}

UINT StringHelper::ScanInt(LPCWSTR str, INT* value_ptr)
{
return StringScanInt(str, value_ptr);
}

UINT StringHelper::ScanInt(LPCSTR str, INT64* value_ptr)
{
return StringScanInt(str, value_ptr);
}

UINT StringHelper::ScanInt(LPCWSTR str, INT64* value_ptr)
{
return StringScanInt(str, value_ptr);
}

UINT StringHelper::ScanUInt(LPCSTR str, UINT* value_ptr, UINT base, UINT len)
{
return StringScanUInt(str, value_ptr, base, len);
}

UINT StringHelper::ScanUInt(LPCWSTR str, UINT* value_ptr, UINT base, UINT len)
{
return StringScanUInt(str, value_ptr, base, len);
}

UINT StringHelper::ScanUInt(LPCSTR str, UINT64* value_ptr, UINT base, UINT len)
{
return StringScanUInt(str, value_ptr, base, len);
}

UINT StringHelper::ScanUInt(LPCWSTR str, UINT64* value_ptr, UINT base, UINT len)
{
return StringScanUInt(str, value_ptr, base, len);
}

LPCSTR StringHelper::Truncate(LPCSTR str, LPCSTR chars)
{
return StringTruncate(str, chars);
}

LPCWSTR StringHelper::Truncate(LPCWSTR str, LPCSTR chars)
{
return StringTruncate(str, chars);
}

UINT StringHelper::Uppercase(LPSTR str)
{
return StringUppercase(str);
}

UINT StringHelper::Uppercase(LPWSTR str)
{
return StringUppercase(str);
}

UINT StringHelper::Uppercase(LPSTR dst, UINT size, LPCSTR str)
{
if(!str)
	return 0;
UINT pos=0;
for(; str[pos]; pos++)
	{
	if(pos+1==size)
		break;
	if(dst)
		dst[pos]=CharHelper::ToCapitalAnsi(str[pos]);
	}
if(dst)
	dst[pos]=0;
return pos;
}

SIZE_T StringHelper::WriteToStream(OutputStream* stream, LPCSTR str)
{
SIZE_T size=0;
StreamWriter writer(stream);
size+=writer.Print(str);
size+=writer.PrintChar('\0');
return size;
}

SIZE_T StringHelper::WriteToStream(OutputStream* stream, LPCWSTR str)
{
SIZE_T size=0;
StreamWriter writer(stream);
size+=writer.Print(str);
size+=writer.PrintChar('\0');
return size;
}
