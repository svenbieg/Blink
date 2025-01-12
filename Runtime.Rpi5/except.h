//==========
// except.h
//==========

#pragma once


//=======
// Using
//=======

#include <asm.h>


//=======
// Types
//=======

CONST_EXPR(UINT, SYNC_EXCEPTION, 0)
CONST_EXPR(UINT, FIQ_EXCEPTION, 1)
CONST_EXPR(UINT, ERROR_EXCEPTION, 2)
CONST_EXPR(UINT, INVALID_EXCEPTION, 3)


//=======
// Frame
//=======

CONST_EXPR(UINT, EXC_REG_COUNT, 32)
CONST_EXPR(UINT, EXC_REG_FRAME, 29)
CONST_EXPR(UINT, EXC_REG_RETURN, 30)
CONST_EXPR(UINT, EXC_REG_STACK, 31)

STRUCT_BEGIN
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X0)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X1)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X2)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X3)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X4)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X5)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X6)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X7)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X8)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X9)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X10)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X11)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X12)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X13)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X14)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X15)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X16)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X17)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X18)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X19)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X20)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X21)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X22)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X23)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X24)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X25)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X26)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X27)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X28)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X29)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, X30)
STRUCT_FIELD(SIZE_T, 8, EXC_FRAME_, SP)
STRUCT_END(EXC_FRAME)


//========
// Common
//========

#ifndef __ASSEMBLER__

ALWAYS_INLINE VOID exc_restore_context(EXC_FRAME* Context)
{
__asm inline volatile("\
ldp x2, x3, [x0, #0x10]\n\
ldp x4, x5, [x0, #0x20]\n\
ldp x6, x7, [x0, #0x30]\n\
ldp x8, x9, [x0, #0x40]\n\
ldp x10, x11, [x0, #0x50]\n\
ldp x12, x13, [x0, #0x60]\n\
ldp x14, x15, [x0, #0x70]\n\
ldp x16, x17, [x0, #0x80]\n\
ldp x18, x19, [x0, #0x90]\n\
ldp x20, x21, [x0, #0xA0]\n\
ldp x22, x23, [x0, #0xB0]\n\
ldp x24, x25, [x0, #0xC0]\n\
ldp x26, x27, [x0, #0xD0]\n\
ldp x28, x29, [x0, #0xE0]\n\
ldp x30, x1, [x0, #0xF0]\n\
mov sp, x1\n\
ldp x0, x1, [x0]\n\
ret");
}

ALWAYS_INLINE VOID exc_resume(EXC_FRAME* Context, SIZE_T Resume, VOID* Argument)
{
SIZE_T stack;
__asm inline volatile("\
ldr %3, [%0, #0xF8]\n\
mov sp, %3\n\
mov x0, %2\n\
br %1":: "r" (Context), "r" (Resume), "r" (Argument), "r" (stack));
}

ALWAYS_INLINE VOID exc_save_context(EXC_FRAME* Context)
{
__asm inline volatile("\
stp x0, x1, [x0, #0x00]\n\
stp x2, x3, [x0, #0x10]\n\
stp x4, x5, [x0, #0x20]\n\
stp x6, x7, [x0, #0x30]\n\
stp x8, x9, [x0, #0x40]\n\
stp x10, x11, [x0, #0x50]\n\
stp x12, x13, [x0, #0x60]\n\
stp x14, x15, [x0, #0x70]\n\
stp x16, x17, [x0, #0x80]\n\
stp x18, x19, [x0, #0x90]\n\
stp x20, x21, [x0, #0xA0]\n\
stp x22, x23, [x0, #0xB0]\n\
stp x24, x25, [x0, #0xC0]\n\
stp x26, x27, [x0, #0xD0]\n\
stp x28, x29, [x0, #0xE0]\n\
mov x1, sp\n\
stp x30, x1, [x0, #0xF0]");
}

#endif
