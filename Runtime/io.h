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

typedef volatile const uint8_t ro8_t;
typedef volatile const uint16_t ro16_t;
typedef volatile const uint32_t ro32_t;
typedef volatile const uint64_t ro64_t;

typedef volatile uint8_t rw8_t;
typedef volatile uint16_t rw16_t;
typedef volatile uint32_t rw32_t;
typedef volatile uint64_t rw64_t;


//========
// Common
//========

inline void io_clear(rw32_t& reg, uint32_t mask) { reg&=~mask; }
inline uint8_t io_read(ro8_t& reg) { return reg; }
inline uint16_t io_read(ro16_t& reg) { return reg; }
inline uint32_t io_read(ro32_t& reg) { return reg; }
inline uint32_t io_read(ro32_t& reg, uint32_t mask) { return reg&mask; }
inline uint32_t io_read(ro32_t& reg, bits32_t const& bits) { return (reg>>bits.shift)&bits.mask; }
inline void io_set(rw32_t& reg, uint32_t mask) { reg|=mask; }
inline void io_write(rw8_t& reg, uint8_t value) { reg=value; }
inline void io_write(rw16_t& reg, uint16_t value) { reg=value; }
inline void io_write(rw32_t& reg, uint32_t value) { reg=value; }
inline void io_write(rw32_t& reg, uint32_t mask, uint32_t value) { uint32_t u=reg; u&=~mask; u|=value; reg=u; }
inline void io_write(rw16_t& reg, bits16_t const& bits, uint16_t value) { uint16_t u=reg; u&=~(bits.mask<<bits.shift); u|=(value<<bits.shift); reg=u; }
inline void io_write(rw32_t& reg, bits32_t const& bits, uint32_t value) { uint32_t u=reg; u&=~(bits.mask<<bits.shift); u|=(value<<bits.shift); reg=u; }
inline void io_write(rw64_t& reg, uint64_t value) { reg=value; }
