//===========
// vectors.S
//===========

.section ".vectors"


//=======
// Using
//=======

#include <config.h>


//==============
// Vector-Table
//==============

#define THUMB 1

.macro ventry label
.word \label+THUMB
.endm

.word __stack_end // Stack-pointer
ventry isr_reset // Entry-point
ventry isr_nmi
ventry isr_hardfault
ventry isr_pagefault
ventry isr_busfault
ventry isr_usagefault
ventry isr_securefault
ventry isr_invalid
ventry isr_invalid
ventry isr_invalid
ventry isr_svcall
ventry isr_debugmon
ventry isr_invalid
ventry isr_pendsv
ventry isr_systick

.altmacro
.macro isr_irqx irq_num
ventry isr_irq\irq_num
.endm

.set irq_num, 0
.rept CONFIG_IRQ_COUNT
isr_irqx %irq_num
.set irq_num, irq_num+1
.endr
