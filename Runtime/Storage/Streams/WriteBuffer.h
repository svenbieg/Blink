//===============
// WriteBuffer.h
//===============

#pragma once


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==============
// Write-Buffer
//==============

class WriteBuffer: public Object
{
public:
	// Con-/Destructors
	~WriteBuffer();
	static inline Handle<WriteBuffer> Create(SIZE_T BlockSize=PAGE_SIZE) { return new WriteBuffer(BlockSize); }

	// Common
	inline SIZE_T Available()const { return m_Written-m_Read; }
	VOID Clear();
	SIZE_T Read(VOID* Buffer, SIZE_T Size);
	VOID Flush();
	SIZE_T Write(VOID const* Buffer, SIZE_T Size);

private:
	// Block
	struct WriteBufferBlock
		{
		WriteBufferBlock* Next;
		SIZE_T Size;
		BYTE Buffer[];
		};

	// Con-/Destructors
	WriteBuffer(UINT BlockSize);

	// Common
	WriteBufferBlock* CreateBlock();
	VOID FreeBlock(WriteBufferBlock* Block);
	VOID FreeBlocks(WriteBufferBlock* First);
	SIZE_T m_BlockSize;
	WriteBufferBlock* m_First;
	WriteBufferBlock* m_Free;
	WriteBufferBlock* m_Last;
	SIZE_T m_Read;
	SIZE_T m_Size;
	SIZE_T m_Written;
};

}}