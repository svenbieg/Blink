//=================
// Configuration.h
//=================

#pragma once


//=======
// Using
//=======

#include "AsmHelper.h"


//===========
// Namespace
//===========

NAMESPACE(Runtime)


//========
// Common
//========

CONST(UINT, CONFIG_CPU_COUNT, 4)
CONST(UINT, CONFIG_STACK_SIZE, 4096)
CONST(UINT, CONFIG_RAM_SIZE, 0x80000000)

NAMESPACE_END
