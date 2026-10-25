//============
// SpiFlash.h
//============

// Winbond Serial NOR Flash Memory W25Q32RVCPJM

#pragma once


//=======
// Using
//=======

#include "Concurrency/Mutex.h"
#include "Devices/Dma/DmaChannel.h"
#include "Storage/Volume.h"
#include "Global.h"


//===========
// Namespace
//===========

namespace Devices {
	 namespace Flash {


//===========
// SPI-Flash
//===========

class SpiFlash: public Global<SpiFlash>, public Storage::Volume
{
public:
	// Settings
	static const UINT BLOCK_COUNT=1024;
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
		return Global::CreateEx(sizeof(FLASH_BUFFER), sizeof(SIZE_T));
		}

	// Volume
	VOID Erase(UINT Block)override;
	UINT GetBlockSize()override;
	WORD GetPageSize(WORD* Spare=nullptr)override;
	UINT64 GetSize()override;
	Handle<Page> ReadPage(UINT Block, WORD Page)override;
	VOID Write(UINT Block, WORD Page, WORD Position, VOID const* Buffer, WORD Size)override;

private:
	// Buffer
	static const SIZE_T BOOT2_SIZE=256;
	typedef struct
		{
		BYTE Boot2[BOOT2_SIZE];
		BYTE Page[PAGE_SIZE];
		}FLASH_BUFFER;

	// ROM-Functions
	using boot2_fn=VOID (*)();
	using flash_connect_fn=VOID (*)();
	using flash_erase_fn=VOID (*)(UINT Offset, SIZE_T Size, UINT BlockSize, BYTE BlockEraseCommand);
	using flash_exit_fn=VOID (*)();
	using flash_flush_fn=VOID (*)();
	using flash_program_fn=VOID (*)(UINT Offset, BYTE const* Data, SIZE_T Size);

	// Con-/Destructors
	SpiFlash(BYTE* Buffer, SIZE_T Size);

	// Common
	VOID DoErase(UINT Offset, SIZE_T Size)noexcept;
	VOID DoWrite(UINT Offset, VOID const* Buffer, SIZE_T Size)noexcept;
	VOID* LookupRomFunction(UINT Code);
	boot2_fn m_Boot2;
	FLASH_BUFFER* m_Buffer;
	Handle<DmaChannel> m_DmaChannel;
	flash_connect_fn m_FlashConnect;
	flash_erase_fn m_FlashErase;
	flash_exit_fn m_FlashExit;
	flash_flush_fn m_FlashFlush;
	flash_program_fn m_FlashProgram;
	Mutex m_Mutex;
};

}}