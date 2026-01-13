//===============
// FlashMemory.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
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
	UINT GetBlockSize()override;
	UINT64 GetSize()override;
	SIZE_T Read(UINT64 Position, VOID* Buffer, SIZE_T Size)override;
	BOOL SetSize(UINT64 Size)override;
	SIZE_T Write(UINT64 Position, VOID const* Buffer, SIZE_T Size)override;

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