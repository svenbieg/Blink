//============
// SpiFlash.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Mutex.h"
#include "Devices/Dma/DmaChannel.h"
#include "Storage/Volume.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Flash {


//===========
// SPI-Flash
//===========

// Winbond Serial NOR Flash Memory W25Q32RVCPJM

class SpiFlash: public Storage::Volume
{
public:
	// Settings
	static const UINT BLOCK_SIZE=4096;
	static const WORD PAGE_SIZE=256;
	static const UINT TOTAL_SIZE=1024*4096;

	// Using
	using DmaChannel=Devices::Dma::DmaChannel;
	using Mutex=Concurrency::Mutex;

	// Friends
	friend Object;

	// Con-/Destructors
	static inline Handle<SpiFlash> Create()
		{
		return Object::Create<SpiFlash>();
		}

	// Volume
	VOID Erase(UINT Block)override;
	UINT GetBlockSize()override;
	WORD GetPageSize(WORD* Spare=nullptr)override;
	UINT64 GetSize()override;
	Handle<Page> ReadPage(UINT Block, WORD Page)override;
	VOID Write(UINT Block, WORD Page, WORD Position, VOID const* Buffer, WORD Size)override;

private:
	// Con-/Destructors
	SpiFlash();

	// Common
	Handle<DmaChannel> m_DmaChannel;
	Mutex m_Mutex;
};

}}