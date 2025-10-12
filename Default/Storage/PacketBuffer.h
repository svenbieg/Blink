//================
// PacketBuffer.h
//================

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Storage/Streams/RandomAccessStream.h"
#include "Storage/packet_buf.h"


//===========
// Namespace
//===========

namespace Storage {


//===============
// Packet-Buffer
//===============

class PacketBuffer: public Streams::RandomAccessStream
{
public:
	// Con-/Destructors
	inline ~PacketBuffer() { Clear(); }
	static inline Handle<PacketBuffer> Create(UINT BlockSize=PAGE_SIZE) { return new PacketBuffer(BlockSize); }

	// Common
	VOID Clear();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Con-/Destructors
	PacketBuffer(UINT BlockSize);

	// Common
	packet_buf m_Buffer;
	Concurrency::Mutex m_Mutex;
	Concurrency::Signal m_Signal;
};

}