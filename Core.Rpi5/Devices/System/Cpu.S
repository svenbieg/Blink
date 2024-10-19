//==============
// CpuContext.S
//==============

.section ".text"


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

.globl cpu_context_set
cpu_context_set:
mov x30, x0
mov x0, x1
mov sp, x2
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
