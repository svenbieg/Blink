//==========
// except.S
//==========

.section ".text"


//=======
// Using
//=======

#include "Runtime/ExceptionFrame.h"


//========
// Common
//========

globl_func exc_restore_context
ldrd r2, r3, [r0, #EXC_FRAME_R2]
ldrd r4, r5, [r0, #EXC_FRAME_R4]
ldrd r6, r7, [r0, #EXC_FRAME_R6]
ldrd r8, r9, [r0, #EXC_FRAME_R8]
ldrd r10, r11, [r0, #EXC_FRAME_R10]
ldrd r12, r1, [r0, #EXC_FRAME_R12]
mov sp, r1
ldr r1, [r0, #EXC_FRAME_PC]
mov lr, r1
ldrd r0, r1, [r0, #EXC_FRAME_R0]
bx lr

globl_func exc_resume
mov r2, r0
mov r0, r1
bx r2

globl_func exc_save_context
strd r0, r1, [r0, #EXC_FRAME_R0]
strd r2, r3, [r0, #EXC_FRAME_R2]
strd r4, r5, [r0, #EXC_FRAME_R4]
strd r6, r7, [r0, #EXC_FRAME_R6]
strd r8, r9, [r0, #EXC_FRAME_R8]
strd r10, r11, [r0, #EXC_FRAME_R10]
mov r1, sp
strd r12, r1, [r0, #EXC_FRAME_R12]
mov r1, lr
strd r1, r1, [r0, #EXC_FRAME_LR]
bx lr
