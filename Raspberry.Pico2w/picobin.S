//===========
// picobin.S
//===========

#define PICOBIN_START 0xFFFFDED3
#define PICOBIN_END 0xAB123579

#define ITEM_IMAGE_TYPE 0x42
#define ITEM_LAST 0xFF

#define IMAGE_TYPE_RP2350 (1<<12)
#define IMAGE_TYPE_SECURE (2<<4)
#define IMAGE_TYPE_EXE (1<<0)


//=============
// Binary-Info
//=============

.section ".picobin"

.word PICOBIN_START

.byte ITEM_IMAGE_TYPE
.byte 1
.hword (IMAGE_TYPE_RP2350|IMAGE_TYPE_SECURE|IMAGE_TYPE_EXE)

.byte ITEM_LAST
.byte 1
.hword 0

.word 0
.word PICOBIN_END
