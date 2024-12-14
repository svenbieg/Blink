//================
// IntelliSense.h
//================

#pragma once


//========
// Common
//========

#define __attribute__(a)
#define __builtin_eh_return_data_regno(n) 0
#define __builtin_isnan(f) false
#define __builtin_isinf(f) false
#define __builtin_offsetof(type, member) 0

#define __STDC_VERSION__ 202000

#define typeid(...) (TypeInfo const*)nullptr


//=======
// Types
//=======

#define __SCHAR_MAX__ 0x7F
#define __SHRT_MAX__ 0x7FFF
#define __INT_MAX__ 0x7FFFFFFF
#define __LONG_MAX__ 0x7FFFFFFFFFFFFFFF
