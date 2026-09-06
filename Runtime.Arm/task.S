//========
// task.S
//========

.section ".text"


//=======
// Using
//=======

#include "Runtime/TaskFrame.h"


//==============
// Task-Context
//==============

globl_func task_restore_context
ldrd r4, r5, [r0, #TASK_FRAME_R4]
ldrd r6, r7, [r0, #TASK_FRAME_R6]
ldrd r8, r9, [r0, #TASK_FRAME_R8]
ldrd r10, r11, [r0, #TASK_FRAME_R10]
add r0, r0, #TASK_FRAME_SIZE
bx lr

globl_func task_save_context
sub r0, r0, #TASK_FRAME_SIZE
strd r4, r5, [r0, #TASK_FRAME_R4]
strd r6, r7, [r0, #TASK_FRAME_R6]
strd r8, r9, [r0, #TASK_FRAME_R8]
strd r10, r11, [r0, #TASK_FRAME_R10]
bx lr
