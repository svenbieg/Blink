//===============
// BitHelper.cpp
//===============

#include "BitHelper.h"


//========
// Common
//========

VOID BitHelper::Clear(VOID* bits, SIZE_T bit_count)
{
MemoryHelper::Zero(bits, bit_count/8);
}

VOID BitHelper::Clear(VOID* bits, SIZE_T bit_count, UINT bit)
{
assert(bit<bit_count);
auto buf=(UINT*)bits;
UINT count=bit_count/32;
UINT pos=bit/32;
UINT mask=1U<<(bit%32);
buf[pos]&=~mask;
}

BOOL BitHelper::Get(VOID const* bits, SIZE_T bit_count, UINT bit)
{
assert(bit<bit_count);
auto buf=(UINT const*)bits;
UINT count=bit_count/32;
UINT pos=bit/32;
UINT mask=1U<<(bit%32);
return (buf[pos]&mask)!=0;
}

VOID BitHelper::Set(VOID* bits, SIZE_T bit_count, UINT bit)
{
assert(bit<bit_count);
auto buf=(UINT*)bits;
UINT count=bit_count/32;
UINT pos=bit/32;
UINT mask=1U<<(bit%32);
buf[pos]|=mask;
}
