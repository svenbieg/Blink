//=================
// FlashMemory.cpp
//=================

#include "FlashMemory.h"


//=======
// Using
//=======

#include "Storage/Buffer.h"
#include <base.h>

using namespace Concurrency;
using namespace Devices::Emmc;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Storage {


//==========
// Settings
//==========

constexpr UINT BLOCK_SIZE=512;
constexpr SIZE_T EMMC_BASE=AXI_EMMC0_BASE;
constexpr UINT EMMC_BASE_CLOCK=200'000'000;
constexpr UINT EMMC_CLOCK_RATE=50'000'000;


//====================
// Master-Boot-Record
//====================

constexpr UINT MBR_ENTRIES=4;
constexpr WORD MBR_MAGIC=0xAA55;
constexpr UINT MBR_SIZE=66;

enum
{
MBR_TYPE_NONE=0x00,
MBR_TYPE_FAT=0x06,
MBR_TYPE_DATABASE=0xDB
};

typedef struct
{
BYTE Flags;
BYTE FirstCHS[3];
BYTE Type;
BYTE LastCHS[3];
UINT Position;
UINT Size;
}MBR_ENTRY;

typedef struct
{
MBR_ENTRY Entries[MBR_ENTRIES];
WORD Magic;
}MBR;


//========
// Volume
//========

UINT FlashMemory::GetBlockSize()
{
return BLOCK_SIZE;
}

UINT64 FlashMemory::GetSize()
{
return m_Size;
}

SIZE_T FlashMemory::Read(UINT64 pos, VOID* buf, SIZE_T size)
{
assert(pos%BLOCK_SIZE==0);
assert(size!=0);
assert(size%BLOCK_SIZE==0);
WriteLock lock(m_Mutex);
UINT64 offset=m_Offset+pos;
UINT block=(UINT)(offset/BLOCK_SIZE);
UINT block_count=(UINT)(size/BLOCK_SIZE);
auto cmd=block_count>1? EmmcCmd::ReadMulti: EmmcCmd::ReadSingle;
m_EmmcHost->Command(cmd, block, nullptr, buf, block_count, BLOCK_SIZE);
return size;
}

BOOL FlashMemory::SetSize(UINT64 size)
{
return size<=m_Size;
}

SIZE_T FlashMemory::Write(UINT64 pos, VOID const* buf, SIZE_T size)
{
assert(pos%BLOCK_SIZE==0);
assert(size!=0);
assert(size%BLOCK_SIZE==0);
WriteLock lock(m_Mutex);
UINT64 offset=m_Offset+pos;
UINT block=(UINT)(offset/BLOCK_SIZE);
UINT block_count=(UINT)(size/BLOCK_SIZE);
auto cmd=block_count>1? EmmcCmd::WriteMulti: EmmcCmd::WriteSingle;
m_EmmcHost->Command(cmd, block, nullptr, (VOID*)buf, block_count, BLOCK_SIZE);
return size;
}


//==========================
// Con-/Destructors Private
//==========================

FlashMemory::FlashMemory():
m_Offset(0),
m_Size(0)
{
m_EmmcHost=EmmcHost::Create(EMMC_BASE, Irq::SdCard);
m_EmmcHost->SetClockRate(EMMC_BASE_CLOCK, EMMC_CLOCK_RATE);
m_EmmcHost->Command(EmmcCmd::GoIdle);
UINT if_cond=m_EmmcHost->Command(EmmcCmd::SendIfCond, IF_COND_DEFAULT);
assert(if_cond==IF_COND_DEFAULT);
UINT op_cond=m_EmmcHost->Command(EmmcAppCmd::SendOpCond, 0);
for(UINT retry=0; retry<10; retry++)
	{
	Task::Sleep(100);
	op_cond=m_EmmcHost->Command(EmmcAppCmd::SendOpCond, op_cond);
	if(FlagHelper::Get(op_cond, OP_COND_SUCCESS))
		break;
	}
if(!FlagHelper::Get(op_cond, OP_COND_SUCCESS))
	throw DeviceNotReadyException();
UINT cid[4];
m_EmmcHost->Command(EmmcCmd::SendCid, 0, cid);
UINT rel_addr=m_EmmcHost->Command(EmmcCmd::SendRelAddr);
UINT rca=BitHelper::Get(rel_addr, RELADDR_RCA);
UINT csd[4];
m_EmmcHost->Command(EmmcCmd::SendCsd, rca<<16, csd);
UINT csd_ver=csd[3]>>30;
UINT size=(((csd[2]&0x3FF)<<2)|csd[1]>>30)+1;
UINT shift=((csd[1]>>15)&0x7)+12;
m_Size=((UINT64)size<<shift)*BLOCK_SIZE;
m_EmmcHost->SelectCard(rca);
auto buf=Buffer::Create(BLOCK_SIZE);
auto buf_ptr=buf->Begin();
MemoryHelper::Zero(buf_ptr, BLOCK_SIZE);
Read(0, buf_ptr, BLOCK_SIZE);
auto mbr=(MBR*)&buf_ptr[BLOCK_SIZE-MBR_SIZE];
if(mbr->Magic!=MBR_MAGIC)
	throw DeviceNotReadyException();
auto entries=mbr->Entries;
if(entries[1].Type!=MBR_TYPE_DATABASE)
	{
	UINT db_pos=entries[0].Position+entries[0].Size;
	UINT db_size=(UINT)(m_Size/BLOCK_SIZE-db_pos);
	MemoryHelper::Zero(&entries[1], 3*sizeof(MBR_ENTRY));
	entries[1].Position=db_pos;
	entries[1].Size=db_size;
	entries[1].Type=MBR_TYPE_DATABASE;
	Write(0, buf_ptr, BLOCK_SIZE);
	}
m_Offset=(UINT64)entries[1].Position*BLOCK_SIZE;
m_Size=(UINT64)entries[1].Size*BLOCK_SIZE;
}

}