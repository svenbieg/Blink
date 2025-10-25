//===============
// FlashMemory.h
//===============

#pragma once


//=======
// Using
//=======

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
	// Using
	using EmmcHost=Devices::Emmc::EmmcHost;

	// Con-/Destructors
	static inline Handle<FlashMemory> Create(EmmcHost* Device, FILE_SIZE Offset, FILE_SIZE Size) { return new FlashMemory(Device, Offset, Size); }

	// Common
	UINT GetBlockSize()override;
	FILE_SIZE GetSize()override;
	SIZE_T Read(FILE_SIZE Position, VOID* Buffer, SIZE_T Size)override;
	BOOL SetSize(FILE_SIZE Size)override;
	SIZE_T Write(FILE_SIZE Position, VOID const* Buffer, SIZE_T Size)override;

private:
	// Con-/Destructors
	FlashMemory(EmmcHost* Device, FILE_SIZE Offset, FILE_SIZE Size);

	// Common
	UINT m_BlockSize;
	Handle<EmmcHost> m_Device;
	FILE_SIZE m_Offset;
	FILE_SIZE m_Size;
};

}