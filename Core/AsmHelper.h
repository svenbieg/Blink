//=============
// AsmHelper.h
//=============

#pragma once


//========
// Macros
//========

#ifdef __ASSEMBLER__

#define CONST_EXPR(Type, Name, Value) Name = Value;

#define ENUM_CLASS(Name)
#define ENUM(Prefix, Name, Value) Prefix##Name = Value;
#define ENUM_END

#define NAMESPACE(Name)
#define NAMESPACE_END

#ifndef __LINKER__
#define STRUCT_BEGIN .pushsection .data; .struct 0
#define STRUCT_FIELD(Type, Size, Prefix, Name) Prefix##Name: .space Size
#define STRUCT_ARRAY(Type, Size, Prefix, Name, Count) Prefix##Name: .space (Size)*(Count)
#define STRUCT_END(Name) Name##_SIZE:; .popsection
#else
#define STRUCT_BEGIN
#define STRUCT_FIELD(Type, Size, Prefix, Name)
#define STRUCT_ARRAY(Type, Size, Prefix, Name, Count)
#define STRUCT_END(Name)
#endif

#else

#include "TypeHelper.h"

#define CONST_EXPR(Type, Name, Value) constexpr Type Name=Value;

#define ENUM_CLASS(Name) enum class Name {
#define ENUM(Prefix, Name, Value) Name=Value,
#define ENUM_END };

#define NAMESPACE(Name) namespace Name {
#define NAMESPACE_END }

#define STRUCT_BEGIN typedef struct {
#define STRUCT_FIELD(Type, Size, Prefix, Name) Type Name;
#define STRUCT_ARRAY(Type, Size, Prefix, Name, Count) Type Name[Count];
#define STRUCT_END(Name) }Name;

#endif
