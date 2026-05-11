//=================
// FlashMemory.cpp
//=================

#include "FlashMemory.h"


//=======
// Using
//=======

#include "Storage/Buffer.h"
#include "Devices/Peripherals.h"

using namespace Concurrency;
using namespace Devices;
using namespace Devices::Emmc;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Storage {


//==========
// Settings
//==========

const UINT SD_BLOCK_SIZE=4096;
const UINT SD_PAGE_SIZE=512;

const SIZE_T EMMC_BASE=AXI_EMMC0_BASE;
const UINT EMMC_BASE_CLOCK=200'000'000;
const UINT EMMC_CLOCK_RATE=50'000'000;


//====================
// Master-Boot-Record
//====================

const UINT MBR_ENTRIES=4;
const WORD MBR_MAGIC=0xAA55;
const UINT MBR_SIZE=66;

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

VOID FlashMemory::Erase(UINT64 pos, UINT size)
{
throw NotImplementedException();
}

WORD FlashMemory::GetAlignment()
{
return 2;
}

UINT FlashMemory::GetBlockSize()
{
return SD_BLOCK_SIZE;
}

UINT FlashMemory::GetPageSize()
{
return SD_PAGE_SIZE;
}

UINT64 FlashMemory::GetSize()
{
return m_Size;
}

VOID FlashMemory::Read(UINT64 pos, VOID* buf, SIZE_T size)
{
assert(pos%SD_PAGE_SIZE==0);
assert(size!=0);
assert(size%SD_PAGE_SIZE==0);
WriteLock lock(m_Mutex);
UINT64 offset=m_Offset+pos;
UINT page=(UINT)(offset/SD_PAGE_SIZE);
UINT page_count=(UINT)(size/SD_PAGE_SIZE);
auto cmd=page_count>1? EmmcCmd::ReadMulti: EmmcCmd::ReadSingle;
m_EmmcHost->Command(cmd, page, nullptr, buf, page_count, SD_PAGE_SIZE);
}

VOID FlashMemory::SetSize(UINT64 size)
{
if(size>m_Size)
	throw OutOfMemoryException();
}

VOID FlashMemory::Write(UINT64 pos, VOID const* buf, SIZE_T size)
{
assert(pos%SD_PAGE_SIZE==0);
assert(size!=0);
assert(size%SD_PAGE_SIZE==0);
WriteLock lock(m_Mutex);
UINT64 offset=m_Offset+pos;
UINT page=(UINT)(offset/SD_PAGE_SIZE);
UINT page_count=(UINT)(size/SD_PAGE_SIZE);
auto cmd=page_count>1? EmmcCmd::WriteMulti: EmmcCmd::WriteSingle;
m_EmmcHost->Command(cmd, page, nullptr, (VOID*)buf, page_count, SD_PAGE_SIZE);
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
m_Size=((UINT64)size<<shift)*SD_PAGE_SIZE;
m_EmmcHost->SelectCard(rca);
auto buf=Buffer::Create(SD_PAGE_SIZE);
auto buf_ptr=buf->Begin();
MemoryHelper::Zero(buf_ptr, SD_PAGE_SIZE);
Read(0, buf_ptr, SD_PAGE_SIZE);
auto mbr=(MBR*)&buf_ptr[SD_PAGE_SIZE-MBR_SIZE];
if(mbr->Magic!=MBR_MAGIC)
	throw DeviceNotReadyException();
auto entries=mbr->Entries;
if(entries[1].Type!=MBR_TYPE_DATABASE)
	{
	UINT db_pos=entries[0].Position+entries[0].Size;
	UINT db_size=(UINT)(m_Size/SD_PAGE_SIZE-db_pos);
	MemoryHelper::Zero(&entries[1], 3*sizeof(MBR_ENTRY));
	entries[1].Position=db_pos;
	entries[1].Size=db_size;
	entries[1].Type=MBR_TYPE_DATABASE;
	Write(0, buf_ptr, SD_PAGE_SIZE);
	}
m_Offset=(UINT64)entries[1].Position*SD_PAGE_SIZE;
m_Size=(UINT64)entries[1].Size*SD_PAGE_SIZE;
}

}