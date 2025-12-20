//=======
// mmu.S
//=======

.section ".text"


//=======
// Using
//=======

#include <base.h>


//=====
// MMU
//=====

#define PAGE_ATT_CACHED 0
#define PAGE_ATT_UNCACHED 1
#define PAGE_ATT_DEVICE 2

.globl mmu_enable
mmu_enable:
ldr x0, =__page_table_start
msr TTBR0_EL1, x0
#define MAIR_ATT_CACHED (0xFF<<(PAGE_ATT_CACHED*8))
#define MAIR_ATT_UNCACHED (0x44<<(PAGE_ATT_UNCACHED*8))
#define MAIR_ATT_DEVICE (0x04<<(PAGE_ATT_DEVICE*8))
ldr x0, =(MAIR_ATT_CACHED|MAIR_ATT_UNCACHED|MAIR_ATT_DEVICE)
msr MAIR_EL1, x0
#define TCR_IPS_4GB (0ULL<<32)
#define TCR_IPS_1TB (2ULL<<32)
#define TCR_EPD1 (1<<23)
#define TCR_TG0_4KB (0<<14)
#define TCR_TG0_64KB (1<<14)
#define TCR_SH0_INNER (3<<12)
#define TCR_ORGN0_WB_WA (1<<10)
#define TCR_IRGN0_WB_WA (1<<8)
#define TCR_T0SZ_128GB (27<<0)
ldr x0, =(TCR_IPS_1TB|TCR_EPD1|TCR_TG0_4KB|TCR_SH0_INNER|TCR_ORGN0_WB_WA|TCR_IRGN0_WB_WA|TCR_T0SZ_128GB)
msr TCR_EL1, x0
isb
tlbi vmalle1
dsb sy
isb
#define SCTLR_I (1<<12)
#define SCTLR_C (1<<2)
#define SCTLR_M (1<<0)
mrs x0, SCTLR_EL1
ldr x1, =(SCTLR_I|SCTLR_C|SCTLR_M)
orr x0, x0, x1
msr SCTLR_EL1, x0
isb
ret


//============
// Page-Table
//============

#define PAGE_TABLE_ENTRIES 512
#define REGION_SIZE 0x40000000 // 0x40000000 (1GB)

#define ENTRY_PXN (1ULL<<53)
#define ENTRY_UXN (1ULL<<54)
#define ENTRY_ACC (1<<10)
#define ENTRY_SHARE_OUT (2<<8)
#define ENTRY_SHARE_IN (3<<8)
#define ENTRY_ATT_CACHED (PAGE_ATT_CACHED<<2)
#define ENTRY_ATT_UNCACHED (PAGE_ATT_UNCACHED<<2)
#define ENTRY_ATT_DEVICE (PAGE_ATT_DEVICE<<2)
#define ENTRY_TYPE_BLOCK 1

#define ENTRY_CACHED (ENTRY_ACC|ENTRY_SHARE_IN|ENTRY_ATT_CACHED|ENTRY_TYPE_BLOCK)
#define ENTRY_UNCACHED (ENTRY_ACC|ENTRY_SHARE_IN|ENTRY_ATT_UNCACHED|ENTRY_TYPE_BLOCK)
#define ENTRY_DEVICE (ENTRY_PXN|ENTRY_UXN|ENTRY_ACC|ENTRY_SHARE_OUT|ENTRY_ATT_DEVICE|ENTRY_TYPE_BLOCK)

.globl mmu_create_page_table
mmu_create_page_table:
ldr x0, =__page_table_start
mov x1, xzr
mov x2, xzr
ldr x3, =REGION_SIZE
_init_cached:
ldr x4, =CACHED_END
ldr x5, =ENTRY_CACHED
1:
orr x6, x1, x5
str x6, [x0, x2, lsl #3]
add x1, x1, x3
add x2, x2, #1
cmp x1, x4
b.lo 1b
_init_uncached:
ldr x7, =CACHED_BASE
ldr x4, =UNCACHED_END
ldr x5, =ENTRY_UNCACHED
1:
orr x6, x7, x5
str x6, [x0, x2, lsl #3]
add x1, x1, x3
add x7, x7, x3
add x2, x2, #1
cmp x1, x4
b.lo 1b
_init_device:
ldr x4, =PCIE_IO_END
ldr x5, =ENTRY_DEVICE
1:
orr x6, x1, x5
str x6, [x0, x2, lsl #3]
add x1, x1, x3
add x2, x2, #1
cmp x1, x4
b.lo 1b
_init_empty:
mov x4, #PAGE_TABLE_ENTRIES
1:
str xzr, [x0, x2, lsl #3]
add x2, x2, #1
cmp x2, x4
b.lo 1b
dsb sy
ret
