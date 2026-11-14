//==========
// config.h
//==========

#pragma once


//=======
// Using
//=======

#include <asm.h>


//========
// Common
//========

CONSTEXPR(UINT, CONFIG_CPU_COUNT, 4)
CONSTEXPR(UINT, CONFIG_STACK_SIZE, 4096)
CONSTEXPR(UINT, CONFIG_RAM_SIZE, 0x80000000)


//========
// Memory
//========

CONSTEXPR(UINT, MMU_ATT_CACHED, 0)
CONSTEXPR(UINT, MMU_ATT_UNCACHED, 1)
CONSTEXPR(UINT, MMU_ATT_DEVICE, 2)
