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


//============
// Conversion
//============

template <std::character _str_t>
UINT StringLowercase(_str_t* str)noexcept
{
if(!str)
	return 0;
UINT pos=0;
while(str[pos])
	{
	WCHAR c=0;
	UINT clen=CharHelper::Read(&str[pos], &c);
	c=CharHelper::ToSmallW(c);
	pos+=CharHelper::Write(&str[pos], clen, c);
	}
return pos;
}

template <std::character _dst_t, std::character _src_t>
UINT StringLowercase(_dst_t* dst, UINT size, _src_t const* src)
{
if(!dst)
	return StringHelper::Length(src);
UINT src_pos=0;
UINT dst_pos=0;
while(src[src_pos])
	{
	WCHAR c=0;
	src_pos+=CharHelper::Read(&src[src_pos], &c);
	c=CharHelper::ToSmallW(c);
	dst_pos+=CharHelper::Write(&dst[dst_pos], size-dst_pos, c);
	}
if(dst_pos==size)
	throw BufferOverrunException();
dst[dst_pos]=0;
return dst_pos;
}

template <std::character _str_t>
UINT StringUppercase(_str_t* str)noexcept
{
if(!str)
	return 0;
UINT pos=0;
while(str[pos])
	{
	WCHAR c=0;
	UINT clen=CharHelper::Read(&str[pos], &c);
	c=CharHelper::ToCapitalW(c);
	pos+=CharHelper::Write(&str[pos], clen, c);
	}
return pos;
}

template <std::character _dst_t, std::character _src_t>
UINT StringUppercase(_dst_t* dst, UINT size, _src_t const* src)
{
if(!dst)
	return StringHelper::Length(src);
UINT src_pos=0;
UINT dst_pos=0;
while(src[src_pos])
	{
	WCHAR c=0;
	src_pos+=CharHelper::Read(&src[src_pos], &c);
	c=CharHelper::ToCapitalW(c);
	dst_pos+=CharHelper::Write(&dst[dst_pos], size-dst_pos, c);
	}
if(dst_pos==size)
	throw BufferOverrunException();
dst[dst_pos]=0;
return dst_pos;
}


//============
// Formatting
//============

enum class Format
{
Char,
Double,
Float,
Hex,
Int,
None,
Percent,
String,
UInt
};

enum class FormatFlags: UINT
{
None=0,
High=1,
Left=2,
Low=4,
Numeric=8,
Precision=16,
Signed=32,
Space=64,
Width=128,
Zero=64
};

template <std::character _str_t, std::unsigned_integral _uint_t>
UINT StringScanUInt(_str_t const* String, _uint_t* Value, UINT Base=10, UINT Length=0);

UINT StringGetFormat(LPCSTR str, Format& format, FormatFlags& flags, UINT& width, UINT& precision)
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
	pos+=StringScanUInt(&str[pos], &width);
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
		UINT len=StringScanUInt(&str[pos], &precision);
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


//========
// Floats
//========

template <std::floating_point _float_t>
INT FloatNormalize(_float_t& f, _float_t max, _float_t min)noexcept
{
if(f==0)
	return 0;
const _float_t exps[]={  1e32f,  1e16f,  1e8f,  1e4f,  1e2f, 1e1f };
const _float_t negs[]={ 1e-31f, 1e-15f, 1e-7f, 1e-3f, 1e-1f, 1e0f };
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

template <> INT FloatNormalize<DOUBLE>(DOUBLE& f, DOUBLE max, DOUBLE min)noexcept
{
if(f==0)
	return 0;
const DOUBLE exps[]={  1e256,  1e128,  1e64,  1e32,  1e16,  1e8,  1e4,  1e2, 1e1 };
const DOUBLE negs[]={ 1e-255, 1e-127, 1e-63, 1e-31, 1e-15, 1e-7, 1e-3, 1e-1, 1e0 };
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

template <std::floating_point _float_t>
VOID FloatSplit(_float_t f, UINT& integral, UINT& decimal, INT& exponent, UINT& width, UINT& precision)noexcept
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


//==========
// Printing
//==========

template <std::character _dst_t, std::character _char_t>
UINT StringPrintChar(_dst_t* dst, UINT size, _char_t c, UINT pos)
{
UINT len=CharHelper::Write((_dst_t*)nullptr, 0, c);
if(!dst)
	return len;
if(pos+len>=size)
	throw BufferOverrunException();
CharHelper::Write(&dst[pos], size-pos, c);
return len;
}

template <std::character _dst_t, std::character _char_t>
UINT StringPrintChar(_dst_t* dst, UINT size, _char_t c, FormatFlags flags, UINT pos)
{
UINT len=CharHelper::Write((_dst_t*)nullptr, 0, c);
if(!dst)
	return len;
if(pos+len>=size)
	throw BufferOverrunException();
if(FlagHelper::Get(flags, FormatFlags::High))
	{
	c=CharHelper::ToCapitalT<_char_t, _char_t>(c);
	}
else if(FlagHelper::Get(flags, FormatFlags::Low))
	{
	c=CharHelper::ToSmallT<_char_t, _char_t>(c);
	}
CharHelper::Write(&dst[pos], size-pos, c);
return len;
}

template <std::character _dst_t, std::character _char_t>
UINT StringPrintChars(_dst_t* dst, UINT size, _char_t c, UINT count, UINT pos)
{
UINT clen=CharHelper::Write((_dst_t*)nullptr, 0, c);
UINT len=count*clen;
if(!dst)
	return len;
if(pos+len>=size)
	throw BufferOverrunException();
for(UINT u=0; u<count; u++)
	pos+=CharHelper::Write(&dst[pos], size-pos, c);
return len;
}

template <std::character _dst_t, std::character _src_t>
UINT StringPrintCapital(_dst_t* dst, UINT size, _src_t const* src, UINT pos)
{
if(!src)
	return 0;
UINT start=pos;
UINT src_pos=0;
UINT dst_pos=pos;
while(src[src_pos])
	{
	WCHAR c=0;
	src_pos+=CharHelper::Read(&src[src_pos], &c);
	if(dst)
		{
		c=CharHelper::ToCapitalW(c);
		dst_pos+=CharHelper::Write(&dst[dst_pos], size-dst_pos, c);
		}
	else
		{
		dst_pos+=CharHelper::Write((_dst_t*)nullptr, 0, c);
		}
	}
return dst_pos-start;
}

template <std::character _dst_t, std::character _src_t>
UINT StringPrintSmall(_dst_t* dst, UINT size, _src_t const* src, UINT pos)
{
if(!src)
	return 0;
UINT start=pos;
UINT src_pos=0;
UINT dst_pos=pos;
while(src[src_pos])
	{
	WCHAR c=0;
	src_pos+=CharHelper::Read(&src[src_pos], &c);
	if(dst)
		{
		c=CharHelper::ToSmallW(c);
		dst_pos+=CharHelper::Write(&dst[dst_pos], size-dst_pos, c);
		}
	else
		{
		dst_pos+=CharHelper::Write((_dst_t*)nullptr, 0, c);
		}
	}
return dst_pos-start;
}

template <std::character _dst_t, std::character _src_t>
UINT StringPrintString(_dst_t* dst, UINT size, _src_t const* src, UINT pos)
{
if(!src)
	return 0;
UINT start=pos;
UINT src_pos=0;
UINT dst_pos=pos;
while(src[src_pos])
	{
	WCHAR c=0;
	src_pos+=CharHelper::Read(&src[src_pos], &c);
	if(dst)
		{
		dst_pos+=CharHelper::Write(&dst[dst_pos], size-dst_pos, c);
		}
	else
		{
		dst_pos+=CharHelper::Write((_dst_t*)nullptr, 0, c);
		}
	}
return dst_pos-start;
}

template <std::character _dst_t, std::character _src_t>
UINT StringPrintString(_dst_t* dst, UINT size, _src_t const* src, FormatFlags flags, UINT width, UINT pos)
{
if(!src)
	return 0;
UINT start=pos;
UINT len=0;
if(width>0)
	len=StringHelper::Length(src);
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left))
		pos+=StringPrintChars(dst, size, ' ', width-len, pos);
	}
if(FlagHelper::Get(flags, FormatFlags::High))
	{
	pos+=StringPrintCapital(dst, size, src, pos);
	}
else if(FlagHelper::Get(flags, FormatFlags::Low))
	{
	pos+=StringPrintSmall(dst, size, src, pos);
	}
else
	{
	pos+=StringPrintString(dst, size, src, pos);
	}
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		pos+=StringPrintChars(dst, size, ' ', width-len, pos);
	}
if(pos<size)
	dst[pos]=0;
return pos-start;
}

template <std::character _dst_t>
UINT StringPrintUInt(_dst_t* dst, UINT size, UINT value, UINT pos)
{
CHAR chars[11];
LPSTR buf=&chars[10];
*buf=0;
do
	{
	*--buf=(CHAR)((value%10)+'0');
	value/=10;
	}
while(value);
return StringPrintString(dst, size, buf, pos);
}

template <std::character _dst_t>
UINT StringPrintUInt(_dst_t* dst, UINT size, UINT64 value, UINT pos)
{
CHAR chars[21];
LPSTR buf=&chars[20];
*buf=0;
do
	{
	*--buf=(CHAR)((value%10)+'0');
	value/=10;
	}
while(value);
return StringPrintString(dst, size, buf, pos);
}

template <std::character _dst_t, std::unsigned_integral _uint_t>
UINT StringPrintUInt(_dst_t* dst, UINT size, _uint_t value, FormatFlags flags, UINT width, UINT pos)
{
UINT len=0;
if(width>0)
	len=StringPrintUInt((LPSTR)nullptr, 0, value, flags, 0, 0);
UINT start=pos;
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&!FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(dst, size, ' ', width-len, pos);
	}
if(FlagHelper::Get(flags, FormatFlags::Signed))
	pos+=StringPrintChar(dst, size, '+', pos);
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(dst, size, '0', width-len, pos);
	}
pos+=StringPrintUInt(dst, size, value, pos);
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		pos+=StringPrintChars(dst, size, ' ', width-len, pos);
	}
if(pos<size)
	dst[pos]=0;
return pos-start;
}

template <std::character _dst_t, std::unsigned_integral _uint_t>
UINT StringPrintHex(_dst_t* dst, UINT size, _uint_t value, UINT pos)
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
return StringPrintString(dst, size, buf, pos);
}

template <std::character _dst_t, std::unsigned_integral _uint_t>
UINT StringPrintHex(_dst_t* dst, UINT size, _uint_t value, FormatFlags flags, UINT width, UINT pos)
{
UINT len=0;
if(width>0)
	len=StringPrintHex((LPSTR)nullptr, 0, value, flags, 0, 0);
UINT start=pos;
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&!FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(dst, size, ' ', width-len, pos);
	}
if(FlagHelper::Get(flags, FormatFlags::Numeric))
	pos+=StringPrintString(dst, size, "0x", pos);
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(dst, size, '0', width-len, pos);
	}
pos+=StringPrintHex(dst, size, value, pos);
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		StringPrintChars(dst, size, ' ', width-len, pos);
	}
if(pos<size)
	dst[pos]=0;
return pos-start;
}

template <std::character _dst_t>
UINT StringPrintInt(_dst_t* dst, UINT size, INT value, UINT pos)
{
BOOL sign=value<0;
if(sign)
	value=-value;
CHAR chars[12];
LPSTR buf=&chars[11];
*buf=0;
do
	{
	*--buf=(CHAR)((value%10)+'0');
	value/=10;
	}
while(value);
if(sign)
	*--buf='-';
return StringPrintString(dst, size, buf, pos);
}

template <std::character _dst_t>
UINT StringPrintInt(_dst_t* dst, UINT size, INT64 value, UINT pos)
{
BOOL sign=value<0;
if(sign)
	value=-value;
CHAR chars[22];
LPSTR buf=&chars[21];
*buf=0;
do
	{
	*--buf=(CHAR)((value%10)+'0');
	value/=10;
	}
while(value);
if(sign)
	*--buf='-';
return StringPrintString(dst, size, buf, pos);
}

template <std::character _dst_t, std::signed_integral _int_t>
UINT StringPrintInt(_dst_t* dst, UINT size, _int_t value, FormatFlags flags, UINT width, UINT pos)
{
UINT len=0;
if(width>0)
	len=StringPrintInt((LPSTR)nullptr, 0, value, flags, 0, 0);
UINT start=pos;
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&!FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(dst, size, ' ', width-len, pos);
	}
if(value<0)
	{
	pos+=StringPrintChar(dst, size, '-', pos);
	value*=-1;
	}
else if(FlagHelper::Get(flags, FormatFlags::Signed))
	{
	pos+=StringPrintChar(dst, size, '+', pos);
	}
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left)&&FlagHelper::Get(flags, FormatFlags::Zero))
		pos+=StringPrintChars(dst, size, '0', width-len, pos);
	}
using _uint_t=std::make_unsigned_t<_int_t>;
pos+=StringPrintUInt(dst, size, (_uint_t)value, pos);
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		pos+=StringPrintChars(dst, size, ' ', width-len, pos);
	}
if(pos<size)
	dst[pos]=0;
return pos-start;
}

template <std::character _dst_t, std::floating_point _float_t>
UINT StringPrintFloat(_dst_t* dst, UINT size, _float_t f, FormatFlags flags, UINT width, UINT precision, UINT pos)
{
UINT start=pos;
if(std::isnan(f))
	{
	pos+=StringPrintString(dst, size, "nan", pos);
	return pos-start;
	}
if(f<0)
	{
	pos+=StringPrintChar(dst, size, '-', pos);
	f*=-1;
	}
else if(FlagHelper::Get(flags, FormatFlags::Signed))
	{
	pos+=StringPrintChar(dst, size, '+', pos);
	}
if(std::isinf(f))
	{
	pos+=StringPrintString(dst, size, "inf", pos);
	return pos-start;
	}
UINT integral=0;
UINT decimal=0;
INT exponent=0;
FloatSplit(f, integral, decimal, exponent, width, precision);
pos+=StringPrintUInt(dst, size, integral, pos);
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
pos+=StringPrintString(dst, size, buf, pos);
if(exponent!=0||FlagHelper::Get(flags, FormatFlags::Numeric))
	{
	pos+=StringPrintChar(dst, size, 'e', pos);
	if(exponent<0)
		{
		pos+=StringPrintChar(dst, size, '-', pos);
		exponent*=-1;
		}
	pos+=StringPrintUInt(dst, size, (UINT)exponent, pos);
	}
if(pos<size)
	dst[pos]=0;
return pos-start;
}

template <std::character _str_t>
UINT StringPrint(_str_t* str, UINT size, LPCSTR format, VariableArguments& args)
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
	fmt+=StringGetFormat(&format[fmt], str_format, flags, width, prec);
	if(str_format==Format::None)
		{
		WCHAR wc=0;
		fmt+=CharHelper::Read(&format[fmt], &wc);
		pos+=StringPrintChar(str, size, wc, pos);
		continue;
		}
	if(str_format==Format::Percent)
		{
		pos+=StringPrintChar(str, size, '%', pos);
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
			pos+=StringPrintInt(str, size, i, flags, width, pos);
			continue;
			}
		case Format::UInt:
			{
			UINT64 u=0;
			if(!args.Get(u))
				break;
			pos+=StringPrintUInt(str, size, u, flags, width, pos);
			continue;
			}
		case Format::Hex:
			{
			UINT64 u=0;
			if(!args.Get(u))
				break;
			pos+=StringPrintHex(str, size, u, flags, width, pos);
			continue;
			}
		case Format::Float:
			{
			FLOAT f=0;
			if(!args.Get(f))
				break;
			pos+=StringPrintFloat(str, size, f, flags, width, prec, pos);
			continue;
			}
		case Format::Double:
			{
			DOUBLE d=0;
			if(!args.Get(d))
				break;
			pos+=StringPrintFloat(str, size, d, flags, width, prec, pos);
			continue;
			}
		case Format::Char:
			{
			WCHAR c=' ';
			if(!args.Get(c))
				break;
			pos+=StringPrintChar(str, size, c, flags, pos);
			continue;
			}
		case Format::String:
			{
			LPCSTR p=nullptr;
			if(args.Get(p))
				{
				pos+=StringPrintString(str, size, p, flags, width, pos);
				continue;
				}
			LPCWSTR pw=nullptr;
			if(args.Get(pw))
				{
				pos+=StringPrintString(str, size, pw, flags, width, pos);
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


//==========
// Scanning
//==========

template <std::character _str_t, std::character _buf_t>
UINT StringScanString(_str_t const* src, _buf_t* dst, UINT size, CHAR esc)
{
if(!src)
	return 0;
UINT src_pos=0;
UINT dst_pos=0;
while(src[src_pos])
	{
	WCHAR c=0;
	src_pos+=CharHelper::Read(&src[src_pos], &c);
	if(CharHelper::Equal(c, esc))
		break;
	if(dst)
		{
		dst_pos+=CharHelper::Write(&dst[dst_pos], size-dst_pos, c);
		}
	else
		{
		dst_pos+=CharHelper::Write((_buf_t*)nullptr, 0, c);
		}
	}
if(dst)
	{
	if(dst_pos==size)
		throw BufferOverrunException();
	dst[dst_pos]=0;
	}
return dst_pos;
}

template <std::character _str_t, std::unsigned_integral _uint_t>
UINT StringScanUInt(_str_t const* str, _uint_t* value_ptr, UINT base, UINT len)
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

template <std::character _str_t, std::signed_integral _int_t>
UINT StringScanInt(_str_t const* str, _int_t* value_ptr)
{
if(value_ptr)
	*value_ptr=0;
if(!str)
	return 0;
UINT pos=0;
BOOL negative=false;
if(CharHelper::Equal(str[pos], '-'))
	{
	pos++;
	if(CharHelper::Equal(str[pos], '-'))
		return 0;
	negative=true;
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

template <std::character _str_t, std::floating_point _float_t>
UINT StringScanFloat(_str_t const* str, _float_t* value_ptr)
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

template <std::character _str_t>
UINT StringScan(_str_t const* str, LPCSTR format, VariableArguments& args)
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
	fmt+=StringGetFormat(&format[fmt], str_format, flags, width, prec);
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
			UINT len=StringScanInt(&str[pos], &i);
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
			UINT len=StringScanUInt(&str[pos], &u, base);
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
			UINT len=StringScanFloat(&str[pos], &f);
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
			UINT len=StringScanFloat(&str[pos], &d);
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
			UINT len=StringScanString(&str[pos], (LPTSTR)nullptr, 0, format[fmt]);
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


//============
// Comparison
//============

template <std::character _str1_t, std::character _str2_t, class... _mode_t>
INT StringCompare(_str1_t const* str1, _str2_t const* str2, UINT len, _mode_t... mode)
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
	UINT len1=StringScanFloat(&str1[pos1], &f1);
	FLOAT f2=0;
	UINT len2=StringScanFloat(&str2[pos2], &f2);
	if(len1==0)
		{
		if(len2==0)
			{
			CHAR c1=0;
			CHAR c2=0;
			len1=CharHelper::Read(&str1[pos1], &c1);
			len2=CharHelper::Read(&str2[pos2], &c2);
			INT cmp=CharHelper::Compare(c1, c2, mode...);
			if(cmp==0)
				{
				pos1+=len1;
				pos2+=len2;
				if(pos2==len)
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

template <std::character _str_t, std::character _find_t, class... _mode_t>
BOOL StringFindChar(_str_t const* str, _find_t c, UINT* pos_ptr, _mode_t... mode)
{
if(!str||!c)
	return false;
BOOL found=false;
UINT pos=0;
while(str[pos])
	{
	WCHAR wc=0;
	pos+=CharHelper::Read(&str[pos], &wc);
	if(CharHelper::Equal(wc, c, mode...))
		{
		found=true;
		break;
		}
	}
if(pos_ptr)
	*pos_ptr=pos;
return found;
}

template <std::character _str_t, std::character _find_t, class... _mode_t>
BOOL StringFindChars(_str_t const* str, _find_t const* find, UINT* pos_ptr, _mode_t... mode)
{
if(!str||!find)
	return false;
UINT pos=0;
while(str[pos])
	{
	WCHAR c=0;
	pos+=CharHelper::Read(&str[pos], &c);
	for(UINT find_pos=0; find[find_pos]; find_pos++)
		{
		WCHAR fc=0;
		find_pos+=CharHelper::Read(&find[find_pos], &fc);
		if(CharHelper::Equal(c, fc, mode...))
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

template <std::character _str_t, std::character _find_t, class... _mode_t>
BOOL StringFindString(_str_t const* str, _find_t const* find, UINT* pos_ptr, _mode_t... mode)
{
if(!str||!find)
	return false;
UINT find_len=StringHelper::Length(find);
if(!find_len)
	return false;
for(UINT pos=0; str[pos]; pos++)
	{
	if(StringCompare(&str[pos], find, find_len, mode...)==0)
		{
		if(pos_ptr)
			*pos_ptr=pos;
		return true;
		}
	}
return false;
}


//==============
// Modification
//==============

template <std::character _dst_t, std::character _src_t, std::character _append_t>
UINT StringAppend(_dst_t* dst, UINT size, _src_t const* str, _append_t const* append)
{
UINT len=StringHelper::Length(str);
if(!len)
	return StringHelper::Copy(dst, size, append);
UINT append_len=StringHelper::Length(append);
if(!append_len)
	return StringHelper::Copy(dst, size, str);
UINT new_len=len+append_len;
if(!dst)
	return new_len;
if(new_len+1>size)
	throw BufferOverrunException();
size-=StringHelper::Copy(dst, size, str);
StringHelper::Copy(&dst[len], size, append);
return new_len;
}

template <std::character _dst_t, std::character _src_t, std::character _insert_t>
UINT StringInsert(_dst_t* dst, UINT size, _src_t const* str, UINT pos, _insert_t const* insert)
{
UINT len=StringHelper::Length(str);
if(pos>len)
	return StringHelper::Copy(dst, size, str);
UINT insert_len=StringHelper::Length(insert);
if(insert_len==0)
	return StringHelper::Copy(dst, size, str);
UINT new_len=len+insert_len;
if(!dst)
	return new_len;
if(new_len+1>size)
	throw BufferOverrunException();
size-=StringHelper::Copy(dst, size, str, pos);
size-=StringHelper::Copy(&dst[pos], size, insert);
StringHelper::Copy(&dst[pos+insert_len], size, &str[pos]);
return new_len;
}

template <std::character _dst_t, std::character _src_t, std::character _find_t, std::character _insert_t, class... _mode_t>
UINT StringReplace(_dst_t* dst, UINT size, _src_t const* str, _find_t const* find, _insert_t const* insert, BOOL repeat, _mode_t... mode)
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
		if(StringCompare(str, find, find_len, mode...)==0)
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

template <std::character _str_t>
_str_t const* StringTruncate(_str_t const* str, LPCSTR chars)
{
UINT len=StringHelper::Length(chars);
while(*str)
	{
	bool trunc=false;
	UINT pos=0;
	while(chars[pos])
		{
		WCHAR c=0;
		pos+=CharHelper::Read(&chars[pos], &c);
		if(CharHelper::Equal(*str, c))
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


//======
// Hash
//======

const UINT BITS_PER_CHAR=5;
const UINT CHARS_PER_HASH=6;

const BYTE HASH_CODE[]=
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
};

template <std::character _str_t>
UINT64 StringHash(_str_t const* str)noexcept
{
if(!str)
	return 0;
UINT64 hash=0;
UINT pos=0;
UINT count=0;
for(; count<CHARS_PER_HASH; count++)
	{
	CHAR c=0;
	pos+=CharHelper::Read(&str[pos], &c);
	if(!c)
		break;
	BYTE b=TypeHelper::Min((BYTE)0x7F, (BYTE)c);
	auto code=HASH_CODE[b];
	hash<<=BITS_PER_CHAR;
	hash|=code;
	}
hash<<=((CHARS_PER_HASH-count)*BITS_PER_CHAR);
return hash;
}


//===============
// String-Helper
//===============

UINT StringHelper::Append(LPSTR dst, UINT size, LPCSTR src, LPCSTR value)noexcept
{
return StringAppend(dst, size, src, value);
}

UINT StringHelper::Append(LPWSTR dst, UINT size, LPCSTR src, LPCSTR value)noexcept
{
return StringAppend(dst, size, src, value);
}

INT StringHelper::Compare(LPCSTR str1, LPCSTR str2, UINT len)noexcept
{
return StringCompare(str1, str2, len);
}

INT StringHelper::Compare(LPCSTR str1, LPCSTR str2, UINT len, CompareMode mode)noexcept
{
return StringCompare(str1, str2, len, mode);
}

INT StringHelper::Compare(LPCSTR str1, LPCWSTR str2, UINT len)noexcept
{
return StringCompare(str1, str2, len);
}

INT StringHelper::Compare(LPCSTR str1, LPCWSTR str2, UINT len, CompareMode mode)noexcept
{
return StringCompare(str1, str2, len, mode);
}

INT StringHelper::Compare(LPCWSTR str1, LPCSTR str2, UINT len)noexcept
{
return StringCompare(str1, str2, len);
}

INT StringHelper::Compare(LPCWSTR str1, LPCSTR str2, UINT len, CompareMode mode)noexcept
{
return StringCompare(str1, str2, len, mode);
}

INT StringHelper::Compare(LPCWSTR str1, LPCWSTR str2, UINT len)noexcept
{
return StringCompare(str1, str2, len);
}

INT StringHelper::Compare(LPCWSTR str1, LPCWSTR str2, UINT len, CompareMode mode)noexcept
{
return StringCompare(str1, str2, len, mode);
}

INT StringHelper::Compare(String const* str1, LPCSTR str2, UINT len)noexcept
{
return StringCompare(str1? str1->Begin(): nullptr, str2, len);
}

INT StringHelper::Compare(String const* str1, LPCSTR str2, UINT len, CompareMode mode)noexcept
{
return StringCompare(str1? str1->Begin(): nullptr, str2, len, mode);
}

INT StringHelper::Compare(String const* str1, LPCWSTR str2, UINT len)noexcept
{
return StringCompare(str1? str1->Begin(): nullptr, str2, len);
}

INT StringHelper::Compare(String const* str1, LPCWSTR str2, UINT len, CompareMode mode)noexcept
{
return StringCompare(str1? str1->Begin(): nullptr, str2, len, mode);
}

INT StringHelper::Compare(String const* str1, String const* str2)noexcept
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
return StringCompare(str1->Begin(), str2->Begin(), 0);
}

INT StringHelper::Compare(String const* str1, String const* str2, UINT len, CompareMode mode)noexcept
{
return StringCompare(str1? str1->Begin(): nullptr, str2? str2->Begin(): nullptr, len, mode);
}

UINT StringHelper::Copy(LPSTR dst, UINT size, LPCSTR src, UINT len)
{
if(!dst)
	return Length(src, len);
if(!src)
	{
	dst[0]=0;
	return 0;
	}
UINT end=size-1;
if(len)
	end=TypeHelper::Min(end, len);
UINT pos=0;
for(; pos<end; pos++)
	{
	if(src[pos]==0)
		break;
	dst[pos]=src[pos];
	}
dst[pos]=0;
return pos;
}

UINT StringHelper::Copy(LPSTR dst, UINT size, LPCWSTR src, UINT len)
{
if(!dst)
	return Length(src, len);
if(!src)
	{
	dst[0]=0;
	return 0;
	}
UINT end=size-1;
if(len)
	end=TypeHelper::Min(end, len);
UINT src_pos=0;
UINT dst_pos=0;
while(dst_pos<end)
	{
	if(src[src_pos]==0)
		break;
	UINT clen=CharHelper::Write(dst, end-dst_pos, src[src_pos]);
	dst_pos+=clen;
	src_pos++;
	}
dst[dst_pos]=0;
return dst_pos;
}

UINT StringHelper::Copy(LPWSTR dst, UINT size, LPCSTR src, UINT len)
{
if(!dst)
	return Length(src, len);
if(!src)
	{
	dst[0]=0;
	return 0;
	}
UINT end=size-1;
if(len)
	end=TypeHelper::Min(end, len);
UINT src_pos=0;
UINT dst_pos=0;
while(dst_pos<end)
	{
	WCHAR wc=0;
	UINT clen=CharHelper::Read(&src[src_pos], &wc);
	if(!wc)
		break;
	dst[dst_pos]=wc;
	dst_pos++;
	src_pos+=clen;
	}
dst[dst_pos]=0;
return dst_pos;
}

UINT StringHelper::Copy(LPWSTR dst, UINT size, LPCWSTR src, UINT len)
{
if(!dst)
	return Length(src, len);
if(!src)
	{
	dst[0]=0;
	return 0;
	}
UINT end=size-1;
if(len)
	end=TypeHelper::Min(end, len);
UINT pos=0;
for(; pos<end; pos++)
	{
	if(src[pos]==0)
		break;
	dst[pos]=src[pos];
	}
dst[pos]=0;
return pos;
}

BOOL StringHelper::FindChar(LPCSTR str, CHAR c, UINT* pos)noexcept
{
return StringFindChar(str, c, pos);
}

BOOL StringHelper::FindChar(LPCSTR str, CHAR c, UINT* pos, CompareMode mode)noexcept
{
return StringFindChar(str, c, pos, mode);
}

BOOL StringHelper::FindChar(LPCWSTR str, CHAR c, UINT* pos)noexcept
{
return StringFindChar(str, c, pos);
}

BOOL StringHelper::FindChar(LPCWSTR str, CHAR c, UINT* pos, CompareMode mode)noexcept
{
return StringFindChar(str, c, pos, mode);
}

BOOL StringHelper::FindChars(LPCSTR str, LPCSTR find, UINT* pos)noexcept
{
return StringFindChars(str, find, pos);
}

BOOL StringHelper::FindChars(LPCSTR str, LPCSTR find, UINT* pos, CompareMode mode)noexcept
{
return StringFindChars(str, find, pos, mode);
}

BOOL StringHelper::FindChars(LPCWSTR str, LPCSTR find, UINT* pos)noexcept
{
return StringFindChars(str, find, pos);
}

BOOL StringHelper::FindChars(LPCWSTR str, LPCSTR find, UINT* pos, CompareMode mode)noexcept
{
return StringFindChars(str, find, pos, mode);
}

BOOL StringHelper::FindString(LPCSTR str, LPCSTR find, UINT* pos)noexcept
{
return StringFindString(str, find, pos);
}

BOOL StringHelper::FindString(LPCSTR str, LPCSTR find, UINT* pos, CompareMode mode)noexcept
{
return StringFindString(str, find, pos, mode);
}

BOOL StringHelper::FindString(LPCSTR str, LPCWSTR find, UINT* pos)noexcept
{
return StringFindString(str, find, pos);
}

BOOL StringHelper::FindString(LPCSTR str, LPCWSTR find, UINT* pos, CompareMode mode)noexcept
{
return StringFindString(str, find, pos, mode);
}

BOOL StringHelper::FindString(LPCWSTR str, LPCSTR find, UINT* pos)noexcept
{
return StringFindString(str, find, pos);
}

BOOL StringHelper::FindString(LPCWSTR str, LPCSTR find, UINT* pos, CompareMode mode)noexcept
{
return StringFindString(str, find, pos, mode);
}

BOOL StringHelper::FindString(LPCWSTR str, LPCWSTR find, UINT* pos)noexcept
{
return StringFindString(str, find, pos);
}

BOOL StringHelper::FindString(LPCWSTR str, LPCWSTR find, UINT* pos, CompareMode mode)noexcept
{
return StringFindString(str, find, pos, mode);
}

UINT64 StringHelper::Hash(LPCSTR str)noexcept
{
return StringHash(str);
}

UINT64 StringHelper::Hash(LPCWSTR str)noexcept
{
return StringHash(str);
}

UINT StringHelper::Insert(LPSTR dst, UINT size, LPCSTR src, UINT pos, LPCSTR value)noexcept
{
return StringInsert(dst, size, src, pos, value);
}

UINT StringHelper::Insert(LPWSTR dst, UINT size, LPCWSTR src, UINT pos, LPCSTR value)noexcept
{
return StringInsert(dst, size, src, pos, value);
}

UINT StringHelper::Length(LPCSTR str)noexcept
{
if(!str)
	return 0;
UINT len=0;
while(str[len])
	len+=CharHelper::Read(&str[len]);
return len;
}

UINT StringHelper::Length(LPCWSTR str)noexcept
{
if(!str)
	return 0;
UINT len=0;
while(str[len])
	len++;
return len;
}

UINT StringHelper::Length(LPCSTR str, UINT max)noexcept
{
if(!str)
	return 0;
if(!max)
	max=UINT_MAX;
UINT len=0;
while(str[len])
	{
	len+=CharHelper::Read(&str[len]);
	if(len>=max)
		return max;
	}
return len;
}

UINT StringHelper::Length(LPCWSTR str, UINT max)noexcept
{
if(!str)
	return 0;
if(!max)
	max=UINT_MAX;
UINT len=0;
while(str[len])
	{
	len++;
	if(len>=max)
		return max;
	}
return len;
}

UINT StringHelper::Length(LPCSTR format, VariableArguments& args)noexcept
{
return StringPrint((LPSTR)nullptr, 0, format, args);
}

UINT StringHelper::Lowercase(LPSTR str)noexcept
{
return StringLowercase(str);
}

UINT StringHelper::Lowercase(LPWSTR str)noexcept
{
return StringLowercase(str);
}

UINT StringHelper::Lowercase(LPSTR dst, UINT size, LPCSTR src)noexcept
{
return StringLowercase(dst, size, src);
}

UINT StringHelper::Lowercase(LPWSTR dst, UINT size, LPCWSTR src)noexcept
{
return StringLowercase(dst, size, src);
}

UINT StringHelper::Print(LPSTR dst, UINT size, LPCSTR format, VariableArguments& args)noexcept
{
return StringPrint(dst, size, format, args);
}

UINT StringHelper::Print(LPWSTR dst, UINT size, LPCSTR format, VariableArguments& args)noexcept
{
return StringPrint(dst, size, format, args);
}

UINT StringHelper::PrintInt(LPSTR dst, UINT size, INT value)noexcept
{
return StringPrintInt(dst, size, value, 0);
}

UINT StringHelper::PrintInt64(LPSTR dst, UINT size, INT64 value)noexcept
{
return StringPrintInt(dst, size, value, 0);
}

UINT StringHelper::PrintUInt(LPSTR dst, UINT size, UINT value)noexcept
{
return StringPrintUInt(dst, size, value, 0);
}

UINT StringHelper::PrintUInt64(LPSTR dst, UINT size, UINT64 value)noexcept
{
return StringPrintUInt(dst, size, value, 0);
}

UINT StringHelper::Replace(LPSTR dst, UINT size, LPCSTR src, LPCSTR find, LPCSTR insert, BOOL repeat)noexcept
{
return StringReplace(dst, size, src, find, insert, repeat);
}

UINT StringHelper::Replace(LPSTR dst, UINT size, LPCSTR src, LPCSTR find, LPCSTR insert, BOOL repeat, CompareMode mode)noexcept
{
return StringReplace(dst, size, src, find, insert, repeat, mode);
}

UINT StringHelper::Replace(LPWSTR dst, UINT size, LPCWSTR src, LPCSTR find, LPCSTR insert, BOOL repeat)noexcept
{
return StringReplace(dst, size, src, find, insert, repeat);
}

UINT StringHelper::Replace(LPWSTR dst, UINT size, LPCWSTR src, LPCSTR find, LPCSTR insert, BOOL repeat, CompareMode mode)noexcept
{
return StringReplace(dst, size, src, find, insert, repeat, mode);
}

UINT StringHelper::Scan(LPCSTR str, LPCSTR format, VariableArguments& args)noexcept
{
return StringScan(str, format, args);
}

UINT StringHelper::Scan(LPCWSTR str, LPCSTR format, VariableArguments& args)noexcept
{
return StringScan(str, format, args);
}

INT StringHelper::ToInt(LPCSTR str)
{
INT value=0;
UINT len=StringScanInt(str, &value);
if(!len||str[len]!=0)
	throw InvalidArgumentException();
return value;
}

INT StringHelper::ToInt(LPCWSTR str)
{
INT value=0;
UINT len=StringScanInt(str, &value);
if(!len||str[len]!=0)
	throw InvalidArgumentException();
return value;
}

BOOL StringHelper::ToInt(LPCSTR str, INT* value_ptr)noexcept
{
UINT len=StringScanInt(str, value_ptr);
if(!len||str[len]!=0)
	return false;
return true;
}

BOOL StringHelper::ToInt(LPCWSTR str, INT* value_ptr)noexcept
{
UINT len=StringScanInt(str, value_ptr);
if(!len||str[len]!=0)
	return false;
return true;
}

INT64 StringHelper::ToInt64(LPCSTR str)
{
INT64 value=0;
UINT len=StringScanInt(str, &value);
if(!len||str[len]!=0)
	throw InvalidArgumentException();
return value;
}

INT64 StringHelper::ToInt64(LPCWSTR str)
{
INT64 value=0;
UINT len=StringScanInt(str, &value);
if(!len||str[len]!=0)
	throw InvalidArgumentException();
return value;
}

BOOL StringHelper::ToInt64(LPCSTR str, INT64* value_ptr)noexcept
{
UINT len=StringScanInt(str, value_ptr);
if(!len||str[len]!=0)
	return false;
return true;
}

BOOL StringHelper::ToInt64(LPCWSTR str, INT64* value_ptr)noexcept
{
UINT len=StringScanInt(str, value_ptr);
if(!len||str[len]!=0)
	return false;
return true;
}

UINT StringHelper::ToUInt(LPCSTR str)
{
UINT value=0;
UINT len=StringScanUInt(str, &value);
if(!len||str[len]!=0)
	throw InvalidArgumentException();
return value;
}

UINT StringHelper::ToUInt(LPCWSTR str)
{
UINT value=0;
UINT len=StringScanUInt(str, &value);
if(!len||str[len]!=0)
	throw InvalidArgumentException();
return value;
}

BOOL StringHelper::ToUInt(LPCSTR str, UINT* value_ptr)noexcept
{
UINT len=StringScanUInt(str, value_ptr);
if(!len||str[len]!=0)
	return false;
return true;
}

BOOL StringHelper::ToUInt(LPCWSTR str, UINT* value_ptr)noexcept
{
UINT len=StringScanUInt(str, value_ptr);
if(!len||str[len]!=0)
	return false;
return true;
}

BOOL StringHelper::ToUInt64(LPCSTR str, UINT64* value_ptr)noexcept
{
UINT len=StringScanUInt(str, value_ptr);
if(!len||str[len]!=0)
	return false;
return true;
}

BOOL StringHelper::ToUInt64(LPCWSTR str, UINT64* value_ptr)noexcept
{
UINT len=StringScanUInt(str, value_ptr);
if(!len||str[len]!=0)
	return false;
return true;
}

UINT64 StringHelper::ToUInt64(LPCSTR str)
{
UINT64 value=0;
UINT len=StringScanUInt(str, &value);
if(!len||str[len]!=0)
	throw InvalidArgumentException();
return value;
}

UINT64 StringHelper::ToUInt64(LPCWSTR str)
{
UINT64 value=0;
UINT len=StringScanUInt(str, &value);
if(!len||str[len]!=0)
	throw InvalidArgumentException();
return value;
}

LPCSTR StringHelper::Truncate(LPCSTR str, LPCSTR chars)noexcept
{
return StringTruncate(str, chars);
}

LPCWSTR StringHelper::Truncate(LPCWSTR str, LPCSTR chars)noexcept
{
return StringTruncate(str, chars);
}

UINT StringHelper::Uppercase(LPSTR str)noexcept
{
return StringUppercase(str);
}

UINT StringHelper::Uppercase(LPWSTR str)noexcept
{
return StringUppercase(str);
}

UINT StringHelper::Uppercase(LPSTR dst, UINT size, LPCSTR src)noexcept
{
return StringUppercase(dst, size, src);
}

UINT StringHelper::Uppercase(LPWSTR dst, UINT size, LPCWSTR src)noexcept
{
return StringUppercase(dst, size, src);
}
