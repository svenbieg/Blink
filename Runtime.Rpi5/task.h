//========
// task.h
//========

#pragma once


//=======
// Using
//=======

#include <asm.h>


//=======
// Frame
//=======

STRUCT_BEGIN
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, ELR)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, X19)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, X20)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, X21)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, X22)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, X23)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, X24)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, X25)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, X26)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, X27)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, X28)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, FPCR)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, FPSR)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, D8)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, D9)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, D10)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, D11)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, D12)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, D13)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, D14)
STRUCT_FIELD(SIZE_T, 8, TASK_FRAME_, D15)
STRUCT_END(TASK_FRAME)


//========
// Common
//========

#ifndef __ASSEMBLER__

#ifdef __cplusplus
extern "C" {
#endif

VOID task_init(VOID** Stack, VOID (*TaskProc)(VOID*), VOID* Parameter);
VOID* task_restore_context(VOID* Stack);
VOID* task_save_context(VOID* Stack);

#ifdef __cplusplus
}
#endif

#endif
