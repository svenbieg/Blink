//=======
// irq.h
//=======

#pragma once


//=======
// Using
//=======

#include <asm.h>


//============
// Interrupts
//============

CONST_EXPR(UINT, IRQ_COUNT, 512);

CONST_EXPR(UINT, IRQ_TASK_SWITCH,		0x00);
CONST_EXPR(UINT, IRQ_SYSTIMER,			0x1E); // 0x1E
//CONST_EXPR(UINT, IRQ_DMA0,			0x70); // 0x50+0x20
//CONST_EXPR(UINT, IRQ_DMA1,			0x71); // 0x51+0x20
//CONST_EXPR(UINT, IRQ_DMA2,			0x72); // 0x52+0x20
//CONST_EXPR(UINT, IRQ_DMA3,			0x73); // 0x53+0x20
//CONST_EXPR(UINT, IRQ_DMA4,			0x74); // 0x54+0x20
//CONST_EXPR(UINT, IRQ_DMA5,			0x75); // 0x55+0x20
//CONST_EXPR(UINT, IRQ_DMA6,			0x76); // 0x56+0x20
//CONST_EXPR(UINT, IRQ_DMA7,			0x77); // 0x57+0x20
//CONST_EXPR(UINT, IRQ_DMA8,			0x78); // 0x58+0x20
//CONST_EXPR(UINT, IRQ_DMA9,			0x79); // 0x59+0x20
//CONST_EXPR(UINT, IRQ_DMA1,			0x7A); // 0x5A+0x20
//CONST_EXPR(UINT, IRQ_DMA1,			0x7B); // 0x5B+0x20
CONST_EXPR(UINT, IRQ_UART10,			0x99); // 0x79+0x20
CONST_EXPR(UINT, IRQ_PCIE_HOST_INTA,	0x105); // 0xE5+0x20
CONST_EXPR(UINT, IRQ_PCIE_HOST_MSI,		0x10A); // 0xEA+0x20
CONST_EXPR(UINT, IRQ_EMMC0,				0x131); // 0x111+0x20
CONST_EXPR(UINT, IRQ_EMMC1,				0x132); // 0x112+0x20


//=======
// Stack
//=======

STRUCT_BEGIN
STRUCT_FIELD(SIZE_T, 8, IRQ_STACK_, SP)
STRUCT_END(IRQ_STACK)


//=======
// Frame
//=======

STRUCT_BEGIN
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X0)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X1)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X2)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X3)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X4)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X5)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X6)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X7)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X8)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X9)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X10)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X11)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X12)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X13)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X14)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X15)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X16)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X17)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X18)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X29)
STRUCT_FIELD(SIZE_T, 8, IRQ_FRAME_, X30)
STRUCT_END(IRQ_FRAME)
