//========
// bits.h
//========

#pragma once


//=======
// Using
//=======

#include <stdint.h>


//===========
// Bit-Field
//===========

typedef struct
{
uint32_t mask;
uint8_t shift;
}bits32_t;


//========
// Common
//========

__always_inline uint32_t bits_get(uint32_t op, uint32_t mask) { return op&mask; }
__always_inline uint32_t bits_get(uint32_t op, bits32_t const& bits) { return (op>>bits.shift)&bits.mask; }
__always_inline void bits_set(uint32_t& op, uint32_t mask) { op|=mask; }
__always_inline void bits_set(uint32_t& op, uint32_t mask, uint32_t value) { op&=~mask; op|=value; }
__always_inline void bits_set(uint32_t& op, bits32_t const& bits, uint32_t value) { op&=~(bits.mask<<bits.shift); op|=(value<<bits.shift); }
__always_inline void bits_set(uint64_t& op, uint64_t mask) { op|=mask; }
