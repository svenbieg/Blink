//================
// StreamBuffer.h
//================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#pragma once


//=======
// Using
//=======

#include "Concurrency/CriticalSection.h"
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
	using CriticalSection=Concurrency::CriticalSection;
	using Signal=Concurrency::Signal;

	// Con-/Destructors
	~StreamBuffer()noexcept;
	static inline Handle<StreamBuffer> Create(SIZE_T ChunkSize=64)
		{
		return new StreamBuffer(ChunkSize);
		}

	// Common
	VOID Clear()noexcept;

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
	VOID FreeChunks(CHUNK* First);
	SIZE_T m_ChunkSize;
	CriticalSection m_CriticalSection;
	CHUNK* m_First;
	CHUNK* m_Free;
	CHUNK* m_Last;
	SIZE_T m_Read;
	Signal m_Signal;
	SIZE_T m_Size;
	SIZE_T m_Written;
};

}}