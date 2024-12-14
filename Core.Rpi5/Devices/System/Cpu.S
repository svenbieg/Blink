//=======
// Cpu.S
//=======

#include "Exceptions.h"


//========
// Common
//========

.globl cpu_compare_and_set
cpu_compare_and_set:
prfm pstl1keep, [x0]
ldaxr w3, [x0]
cmp w3, w1
bne 1f
stxr w3, w2, [x0]
cbz w3, 2f
1:
mov x0, #0
ret
2:
mov x0, #1
ret

.globl cpu_context_restore
cpu_context_restore:
ldr d31, [x0, EXC_FRAME_D31]
ldr d30, [x0, EXC_FRAME_D30]
ldp d28, d29, [x0, EXC_FRAME_D28]
ldp d26, d27, [x0, EXC_FRAME_D26]
ldp d24, d25, [x0, EXC_FRAME_D24]
ldp d22, d23, [x0, EXC_FRAME_D22]
ldp d20, d21, [x0, EXC_FRAME_D20]
ldp d18, d19, [x0, EXC_FRAME_D18]
ldp d16, d17, [x0, EXC_FRAME_D16]
ldp d14, d15, [x0, EXC_FRAME_D14]
ldp d12, d13, [x0, EXC_FRAME_D12]
ldp d10, d11, [x0, EXC_FRAME_D10]
ldp d8, d9, [x0, EXC_FRAME_D8]
ldp d6, d7, [x0, EXC_FRAME_D6]
ldp d4, d5, [x0, EXC_FRAME_D4]
ldp d2, d3, [x0, EXC_FRAME_D2]
ldp d0, d1, [x0, EXC_FRAME_D0]
ldp x29, x30, [x0, EXC_FRAME_SP]
mov sp, x29
ldp x28, x29, [x0, EXC_FRAME_X28]
ldp x26, x27, [x0, EXC_FRAME_X26]
ldp x24, x25, [x0, EXC_FRAME_X24]
ldp x22, x23, [x0, EXC_FRAME_X22]
ldp x20, x21, [x0, EXC_FRAME_X20]
ldp x18, x19, [x0, EXC_FRAME_X18]
ldp x16, x17, [x0, EXC_FRAME_X16]
ldp x14, x15, [x0, EXC_FRAME_X14]
ldp x12, x13, [x0, EXC_FRAME_X12]
ldp x10, x11, [x0, EXC_FRAME_X10]
ldp x8, x9, [x0, EXC_FRAME_X8]
ldp x6, x7, [x0, EXC_FRAME_X6]
ldp x4, x5, [x0, EXC_FRAME_X4]
ldp x2, x3, [x0, EXC_FRAME_X2]
ldp x0, x1, [x0, EXC_FRAME_X0]
ret

.globl cpu_context_save
cpu_context_save:
stp x0, x1, [x0, EXC_FRAME_X0]
stp x2, x3, [x0, EXC_FRAME_X2]
stp x4, x5, [x0, EXC_FRAME_X4]
stp x6, x7, [x0, EXC_FRAME_X6]
stp x8, x9, [x0, EXC_FRAME_X8]
stp x10, x11, [x0, EXC_FRAME_X10]
stp x12, x13, [x0, EXC_FRAME_X12]
stp x14, x15, [x0, EXC_FRAME_X14]
stp x16, x17, [x0, EXC_FRAME_X16]
stp x18, x19, [x0, EXC_FRAME_X18]
stp x20, x21, [x0, EXC_FRAME_X20]
stp x22, x23, [x0, EXC_FRAME_X22]
stp x24, x25, [x0, EXC_FRAME_X24]
stp x26, x27, [x0, EXC_FRAME_X26]
stp x28, x29, [x0, EXC_FRAME_X28]
mov x1, sp
stp x30, x1, [x0, EXC_FRAME_X30]
stp d0, d1, [x0, EXC_FRAME_D0]
stp d2, d3, [x0, EXC_FRAME_D2]
stp d4, d5, [x0, EXC_FRAME_D4]
stp d6, d7, [x0, EXC_FRAME_D6]
stp d8, d9, [x0, EXC_FRAME_D8]
stp d10, d11, [x0, EXC_FRAME_D10]
stp d12, d13, [x0, EXC_FRAME_D12]
stp d14, d15, [x0, EXC_FRAME_D14]
stp d16, d17, [x0, EXC_FRAME_D16]
stp d18, d19, [x0, EXC_FRAME_D18]
stp d20, d21, [x0, EXC_FRAME_D20]
stp d22, d23, [x0, EXC_FRAME_D22]
stp d24, d25, [x0, EXC_FRAME_D24]
stp d26, d27, [x0, EXC_FRAME_D26]
stp d28, d29, [x0, EXC_FRAME_D28]
str d30, [x0, EXC_FRAME_D30]
str d31, [x0, EXC_FRAME_D31]
ret

.globl cpu_context_set
cpu_context_set:
mov x30, x0
mov x0, x1
mov sp, x2
ret

.globl cpu_id
cpu_id:
mrs x0, mpidr_el1
lsr x0, x0, 8
and x0, x0, 3
ret

.globl cpu_power_on
cpu_power_on:
lsl x0, x0, #8
mov x1, x0
ldr x0, =0xC4000003
ldr x2, =_start
mov x3, #0
smc	#0
ret
