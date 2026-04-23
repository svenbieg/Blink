//=========
// float.S
//=========

.syntax unified

#include "AsmHelper.h"
#include "dcp_instr.inc.S"
#include "dcp_canned.inc.S"


//========
// Common
//========

.macro float_func name
globl_func \name
mrs r4, PRIMASK
cpsid i
.endm

.macro float_ret
msr PRIMASK, r4
bx lr
.endm


//============
// Conversion
//============

float_func __aeabi_d2f
dcp_double2float_m r0, r0, r1
float_ret

float_func __aeabi_d2lz
dcp_double2int_m r0, r0, r1
float_ret

float_func __aeabi_d2uiz
dcp_double2uint_m r0, r0, r1
float_ret

float_func __aeabi_f2d
dcp_float2double_m r0, r1, r0
float_ret

float_func __aeabi_f2uiz
dcp_float2uint_m r0, r0
float_ret

float_func __aeabi_i2f
dcp_int2float_m r0, r0
float_ret

float_func __aeabi_l2f
cmp r1, #0
bge 1f
mvns r0, r0
mvns r1, r1
adds r0, r0, #1
adc r1, r1, #0
mov r2, r0
dcp_uint2float_m r0, r1
movw r3, #0
movt r3, #1
dcp_uint2float_m r3, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_uint2float_m r3, r2
dcp_fadd_m r0, r0, r3
eor r0, r0, #0x80000000
float_ret
1:
mov r2, r0
dcp_uint2float_m r0, r1
movw r3, #0
movt r3, #1
dcp_uint2float_m r3, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_uint2float_m r3, r2
dcp_fadd_m r0, r0, r3
float_ret

float_func __aeabi_l2d
cmp r1, #0
bge 1f
mvns r0, r0
mvns r1, r1
adds r0, r0, #1
adc r1, r1, #0
mov r2, r0
dcp_uint2float_m r0, r1
movw r3, #0
movt r3, #1
dcp_uint2float_m r3, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_uint2float_m r3, r2
dcp_fadd_m r0, r0, r3
dcp_float2double_m r0, r1, r0
eor r1, r1, #0x80000000
float_ret
1:
mov r2, r0
dcp_uint2float_m r0, r1
movw r3, #0
movt r3, #1
dcp_uint2float_m r3, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_uint2float_m r3, r2
dcp_fadd_m r0, r0, r3
dcp_float2double_m r0, r1, r0
float_ret

float_func __aeabi_ui2f
dcp_uint2float_m r0, r0
float_ret

float_func __aeabi_ui2d
dcp_uint2double_m r0, r1, r0
float_ret

float_func __aeabi_ul2f
cmp r1, #0
bne 1f
dcp_uint2float_m r0, r0
float_ret
1:
mov r2, r0
dcp_uint2float_m r0, r1
movw r3, #0
movt r3, #1
dcp_uint2float_m r3, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_uint2float_m r3, r2
dcp_fadd_m r0, r0, r3
float_ret

float_func __aeabi_ul2d
cmp r1, #0
bne 1f
dcp_uint2float_m r0, r0
dcp_float2double_m r0, r1, r0
float_ret
1:
mov r2, r0
dcp_uint2float_m r0, r1
movw r3, #0
movt r3, #1
dcp_uint2float_m r3, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_fmul_m r0, r0, r3, r0, r3
dcp_uint2float_m r3, r2
dcp_fadd_m r0, r0, r3
dcp_float2double_m r0, r1, r0
float_ret


//============
// Comparison
//============

float_func __aeabi_dcmpeq
dcp_dcmp_m r0, r0, r1, r2, r3
ubfx r0, r0, #30, #1
float_ret

float_func __aeabi_dcmpge
dcp_dcmp_m r15, r0, r1, r2, r3
ite hs
movhs r0, #1
movlo r0, #0
float_ret

float_func __aeabi_dcmpgt
dcp_dcmp_m r15, r0, r1, r2, r3
ite hi
movhi r0, #1
movls r0, #0
float_ret

float_func __aeabi_dcmple
dcp_dcmp_m r15, r2, r3, r0, r1
ite hs
movhs r0, #1
movlo r0, #0
float_ret

float_func __aeabi_dcmplt
dcp_dcmp_m r15, r2, r3, r0, r1
ite hi
movhi r0,#1
movls r0,#0
float_ret

float_func __aeabi_dcmpun
dcp_dcmp_m r0, r0, r1, r2, r3
ubfx r0, r0, #28, #1
float_ret

float_func __aeabi_fcmpeq
dcp_fcmp_m r0, r0, r1
ubfx r0, r0, #30, #1
float_ret

float_func __aeabi_fcmpge
dcp_fcmp_m r15, r0, r1
ite hs
movhs r0, #1
movlo r0, #0
float_ret

float_func __aeabi_fcmpgt
dcp_fcmp_m r15, r0, r1
ite hi
movhi r0, #1
movls r0, #0
float_ret

float_func __aeabi_fcmple
dcp_fcmp_m r15, r1, r0
ite hs
movhs r0, #1
movlo r0, #0
float_ret

float_func __aeabi_fcmplt
dcp_fcmp_m r15, r1, r0
ite hi
movhi r0, #1
movls r0, #0
float_ret

float_func __aeabi_fcmpun
dcp_fcmp_m r0, r0, r1
ubfx r0, r0, #28, #1
float_ret


//============
// Arithmetic
//============

float_func __aeabi_dadd
dcp_dadd_m r0, r1, r0, r1, r2, r3
float_ret

float_func __aeabi_ddiv
dcp_ddiv_m r0, r1, r0, r1, r2, r3, r0, r1, r2, r3, r12
float_ret

float_func __aeabi_dmul
push {r4, r14}
dcp_dmul_m r0, r1, r0, r1, r2, r3, r0, r1, r2, r3, r4, r12, r14
pop {r4, lr}
float_ret

float_func __aeabi_dsub
dcp_dsub_m r0, r1, r0, r1, r2, r3
float_ret

float_func __aeabi_fadd
dcp_fadd_m r0, r0, r1
float_ret

float_func __aeabi_fdiv
dcp_fdiv_m r0, r0, r1, r0, r1, r2, r3
float_ret

float_func __aeabi_fmul
dcp_fmul_m r0, r0, r1, r0, r1
float_ret

float_func __aeabi_fsub
dcp_fsub_m r0, r0, r1
float_ret
