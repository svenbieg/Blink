//==========
// config.h
//==========

#pragma once


//=======
// Using
//=======

#include <asm.h>

#ifndef __ASSEMBLER__
#include "Platform.h"
#endif


//========
// Common
//========

CONST_EXPR(UINT, CACHE_LINE_SIZE, 64);
CONST_EXPR(UINT, CPU_COUNT, 4);
CONST_EXPR(UINT, STACK_SIZE, 4096);
CONST_EXPR(UINT, RAM_SIZE, 0x80000000);
