//==========
// config.h
//==========

#pragma once


//=======
// Using
//=======

#include <asm.h>
#include <stdint.h>


//========
// Common
//========

CONST_EXPR(uint32_t, CACHE_LINE_SIZE, 64);
CONST_EXPR(uint32_t, CPU_COUNT, 4);
CONST_EXPR(uint32_t, STACK_SIZE, 4096);
CONST_EXPR(uint32_t, RAM_SIZE, 0x80000000);
