//======
// io.h
//======

#pragma once


//=======
// Using
//=======

#include <bits.h>


//===========
// Registers
//===========

typedef volatile const uint32_t ro32_t;
typedef volatile uint32_t rw32_t;


//========
// Common
//========

__always_inline uint32_t io_read(ro32_t& reg) { return reg; }
__always_inline uint32_t io_read(ro32_t& reg, uint32_t mask) { return reg&mask; }
__always_inline uint32_t io_read(ro32_t& reg, bits32_t const& bits) { return (reg>>bits.shift)&bits.mask; }
__always_inline void io_write(rw32_t& reg, uint32_t value) { reg=value; }
__always_inline void io_write(rw32_t& reg, uint32_t mask, uint32_t value) { uint32_t u=reg; u&=~mask; u|=value; reg=u; }
__always_inline void io_write(rw32_t& reg, bits32_t const& bits, uint32_t value) { uint32_t u=reg; u&=~(bits.mask<<bits.shift); u|=(value<<bits.shift); reg=u; }
