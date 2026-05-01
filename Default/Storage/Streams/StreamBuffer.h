//================
// StreamBuffer.h
//================

#pragma once


//=======
// Using
//=======

#include "Concurrency/Mutex.h"
#include "Concurrency/Signal.h"
#include "Storage/Streams/RandomAccessStream.h"
#include "Handle.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//===============
// Stream-Buffer
//===============

class StreamBuffer: public Object, public RandomAccessStream
{
public:
	// Using
	using Mutex=Concurrency::Mutex;
	using Signal=Concurrency::Signal;

	// Friends
	friend Object;

	// Con-/Destructors
	~StreamBuffer();
	static inline Handle<StreamBuffer> Create(SIZE_T ChunkSize=64) { return new StreamBuffer(ChunkSize); }

	// Common
	VOID Clear();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Chunk
	struct CHUNK
		{
		CHUNK* Next;
		SIZE_T Size;
		BYTE Buffer[];
		};

	// Con-/Destructors
	StreamBuffer(SIZE_T ChunkSize);

	// Common
	CHUNK* CreateChunk();
	VOID FreeChunk();
	VOID FreeChunks(CHUNK* First);
	SIZE_T m_ChunkSize;
	CHUNK* m_First;
	CHUNK* m_Last;
	Mutex m_Mutex;
	SIZE_T m_Read;
	Signal m_Signal;
	SIZE_T m_Size;
	SIZE_T m_Written;
};

}}