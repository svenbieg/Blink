//===============
// FlashMemory.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/Emmc/EmmcHost.h"
#include "Storage/Volume.h"


//===========
// Namespace
//===========

namespace Storage {


//==============
// Flash-Memory
//==============

class FlashMemory: public Volume
{
public:
	// Con-/Destructors
	static inline Handle<FlashMemory> Create() { return Object::Create<FlashMemory>(); }

	// Common
	VOID Erase(UINT64 Position, UINT Size)override;
	WORD GetAlignment()override;
	UINT GetBlockSize()override;
	UINT GetPageSize()override;
	UINT64 GetSize()override;
	VOID Read(UINT64 Position, VOID* Buffer, SIZE_T Size)override;
	VOID SetSize(UINT64 Size)override;
	VOID Write(UINT64 Position, VOID const* Buffer, SIZE_T Size)override;

private:
	// Using
	using EmmcHost=Devices::Emmc::EmmcHost;

	// Friends
	friend Object;

	// Con-/Destructors
	FlashMemory();

	// Common
	Handle<EmmcHost> m_EmmcHost;
	Concurrency::Mutex m_Mutex;
	UINT64 m_Offset;
	UINT64 m_Size;
};

}