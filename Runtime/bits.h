//========
// bits.h
//========

#pragma once


//=======
// Using
//=======

#include <attr.h>
#include <stdint.h>


//===========
// Bit-Field
//===========

typedef struct
{
uint16_t mask;
uint8_t shift;
}bits16_t;

typedef struct
{
uint32_t mask;
uint8_t shift;
}bits32_t;

typedef struct
{
uint64_t mask;
uint8_t shift;
}bits64_t;


//========
// Common
//========

inline uint32_t bits_count(uint64_t value)
{
uint32_t bits=0;
while(value>0)
	{
	value>>=1;
	bits++;
	}
return bits;
}

__always_inline void bits_clear(uint32_t& op, uint32_t mask) { op&=~mask; }
__always_inline uint32_t  bits_get(uint32_t op, uint32_t mask) { return op&mask; }
__always_inline uint32_t bits_get(uint32_t op, bits32_t const& bits) { return (op>>bits.shift)&bits.mask; }
__always_inline void bits_set(uint32_t& op, uint32_t mask) { op|=mask; }
__always_inline void bits_set(uint32_t& op, uint32_t mask, uint32_t value) { op&=~mask; op|=value; }
__always_inline void bits_set(uint32_t& op, bits32_t const& bits, uint32_t value) { op&=~(bits.mask<<bits.shift); op|=(value<<bits.shift); }
__always_inline void bits_set(uint64_t& op, uint64_t mask) { op|=mask; }
__always_inline void bits_write(uint32_t& op, uint32_t mask) { op=mask; }
