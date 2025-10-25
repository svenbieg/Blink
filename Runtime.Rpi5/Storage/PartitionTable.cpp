//====================
// PartitionTable.cpp
//====================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/PartitionTable.h"


//===========
// Namespace
//===========

namespace Storage {


//====================
// Master-Boot-Record
//====================

constexpr WORD MBR_MAGIC=0xAA55;

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
MBR_ENTRY Entries[PARTITION_ENTRIES];
WORD Magic;
}MBR;


//========
// Common
//========

UINT PartitionTable::Initialize(Volume* volume, PARTITION_TABLE* info)
{
UINT count=Read(volume, info);
if(!count)
	throw DeviceNotReadyException();
FILE_SIZE capacity=volume->GetSize();
auto last=&info->Entries[count-1];
FILE_SIZE last_pos=last->Position;
FILE_SIZE last_size=last->Size;
FILE_SIZE last_end=last_pos+last_size;
if(last->Type==PartitionType::Database)
	{
	if(last_end==capacity)
		return count;
	last->Size=capacity-last_pos;
	Write(volume, info);
	return count;
	}
auto dbase=&info->Entries[count];
dbase->Position=last_end;
dbase->Size=capacity-last_end;
dbase->Type=PartitionType::Database;
Write(volume, info);
return count+1;
}

UINT PartitionTable::Read(Volume* volume, PARTITION_TABLE* info)
{
MemoryHelper::Fill(info, sizeof(PARTITION_TABLE), 0);
UINT block_size=volume->GetBlockSize();
FILE_SIZE mbr_pos=block_size-66;
MBR mbr;
volume->Read(mbr_pos, &mbr, 66);
if(mbr.Magic!=MBR_MAGIC)
	return 0;
UINT count=0;
for(UINT u=0; u<PARTITION_ENTRIES; u++)
	{
	auto entry=&mbr.Entries[u];
	if(entry->Type==0)
		break;
	FILE_SIZE pos=(FILE_SIZE)entry->Position*block_size;
	FILE_SIZE size=(FILE_SIZE)entry->Size*block_size;
	info->Entries[u].Position=pos;
	info->Entries[u].Size=size;
	info->Entries[u].Type=(PartitionType)entry->Type;
	count++;
	}
return count;
}

VOID PartitionTable::Write(Volume* volume, PARTITION_TABLE const* info)
{
UINT block_size=volume->GetBlockSize();
MBR mbr;
MemoryHelper::Fill(&mbr, sizeof(MBR), 0);
for(UINT u=0; u<PARTITION_ENTRIES; u++)
	{
	auto entry=&info->Entries[u];
	if(entry->Type==PartitionType::None)
		break;
	mbr.Entries[u].Position=(UINT)(entry->Position/block_size);
	mbr.Entries[u].Size=(UINT)(entry->Size/block_size);
	mbr.Entries[u].Type=(BYTE)entry->Type;
	}
mbr.Magic=MBR_MAGIC;
FILE_SIZE mbr_pos=block_size-66;
volume->Write(mbr_pos, &mbr, 66);
}

}