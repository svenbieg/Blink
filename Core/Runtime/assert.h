//==========
// assert.h
//==========

#pragma once


//=======
// Using
//=======

#include <stdlib.h>


//========
// Common
//========

#ifdef _DEBUG

#define assert(cond)if(!(cond)){ abort(); }

#else

#define assert(...)

#endif
