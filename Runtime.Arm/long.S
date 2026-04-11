//========
// long.S
//========

.syntax unified

#include <asm.h>


//=================
// Signded Divison
//=================

// r0: Dividend Low
// r1: Dividend High
// r2: Divisor Low
// r3: Divisor High

globl_func __aeabi_ldivmod
push {r4-r11, lr}

.save_signs:
mov r4, r1
mov r5, r3
lsr r6, r4, #31
lsr r7, r5, #31

.dividend_abs:
tst r6, #1
beq .divident_abs_done
mvn r8, r0
mvn r9, r1
add r8, r8, #1
adc r9, r9, #0
mov r0, r8
mov r1, r9
.divident_abs_done:

.divisor_abs:
tst r7, #1
beq .divisor_abs_done
mvn r8, r2
mvn r9, r3
add r8, r8, #1
adc r9, r9, #0
mov r2, r8
mov r3, r9
.divisor_abs_done:

bl __aeabi_uldivmod

.fix_quot:
eor r8, r6, r7
tst r8, #1
beq .fix_quot_done
mvn r8, r0
mvn r9, r1
add r8, r8, #1
adc r9, r9, #0
mov r0, r8
mov r1, r9
.fix_quot_done:

.fix_remainder:
tst r6, #1
beq .fix_remainder_done
mvn r8, r2
mvn r9, r3
add r8, r8, #1
adc r9, r9, #0
mov r2, r8
mov r3, r9
.fix_remainder_done:

// r0: Quotient Low
// r1: Quotient High
// r2: Remainder Low
// r3: Remainder High
pop {r4-r11, pc}


//===================
// Unsigned Division
//===================

// r0: Dividend Low
// r1: Dividend High
// r2: Divisor Low
// r3: Divisor High

globl_func __aeabi_uldivmod
push {r4-r11, lr}

.div_by_zero:
orr r4, r2, r3
cmp r4, #0
bne .init_vars
mov r0, #0
mov r1, #0
pop {r4-r11, pc}

.init_vars:
mov r4, #0
mov r5, #0
mov r6, #0
mov r7, #0
mov r9, #64

.loop:
lsls r0, r0, #1
adcs r1, r1, r1
adcs r6, r6, r6
adc r7, r7, r7
subs r10, r6, r2
sbcs r11, r7, r3
blo .next_bit
mov r6, r10
mov r7, r11
orr r0, r0, #1
.next_bit:
subs r9, r9, #1
bne .loop

mov r2, r6
mov r3, r7

// r0: Quotient Low
// r1: Quotient High
// r2: Remainder Low
// r3: Remainder High
pop {r4-r11, pc}
