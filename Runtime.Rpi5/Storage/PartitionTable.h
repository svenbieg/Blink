//==================
// PartitionTable.h
//==================

#pragma once


//=======
// Using
//=======

#include "Storage/Volume.h"


//===========
// Namespace
//===========

namespace Storage {


//================
// Partition-Info
//================

constexpr UINT PARTITION_ENTRIES=4;

enum class PartitionType
{
None=0,
Fat=0x06,
Database=0xDB
};

typedef struct
{
FILE_SIZE Position;
FILE_SIZE Size;
PartitionType Type;
}PARTITION_INFO;

typedef struct
{
PARTITION_INFO Entries[PARTITION_ENTRIES];
}PARTITION_TABLE;


//=================
// Partition-Table
//=================

class PartitionTable
{
public:
	// Common
	static UINT Initialize(Volume* Volume, PARTITION_TABLE* Info);
	static UINT Read(Volume* Volume, PARTITION_TABLE* Info);
	static VOID Write(Volume* Volume, PARTITION_TABLE const* Info);
};

}