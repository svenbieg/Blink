//================
// PacketBuffer.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/RandomAccessStream.h"


//===========
// Namespace
//===========

namespace Storage {


//===============
// Packet-Buffer
//===============

class PacketBuffer: public Storage::Streams::RandomAccessStream
{
public:
	// Con-/Destructors
	static Handle<PacketBuffer> Create(SIZE_T Size);

	// Common
	inline VOID Append(PacketBuffer* Buffer) { m_Next=Buffer; }
	Handle<PacketBuffer> Begin();
	inline SIZE_T Empty() { return m_Size-m_Written; }
	Handle<PacketBuffer> End();
	Handle<PacketBuffer> Next()const { return m_Next; }
	static SIZE_T Read(Handle<PacketBuffer>* PacketBuffer, VOID* Buffer, SIZE_T Size);
	static SIZE_T TotalAvailable(PacketBuffer* PacketBuffer);
	static SIZE_T Write(Handle<PacketBuffer>* PacketBuffer, VOID const* Buffer, SIZE_T Size, SIZE_T BlockSize=PAGE_SIZE);

	// Input-Stream
	SIZE_T Available()override { return m_Written-m_Read; }
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Con-/Destructors
	PacketBuffer(BYTE* Buffer, SIZE_T Size);

	// Common
	BYTE* m_Buffer;
	Handle<PacketBuffer> m_Next;
	SIZE_T m_Read;
	SIZE_T m_Size;
	SIZE_T m_Written;
};

}