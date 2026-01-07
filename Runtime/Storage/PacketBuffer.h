//================
// PacketBuffer.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/Stream.h"


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
	static Handle<PacketBuffer> Create(UINT Size) { return Object::CreateEx<PacketBuffer>(Size); }

	// Common
	inline BYTE* Begin()const { return m_Buffer; }
	inline UINT GetSize()const { return m_Size; }
	Handle<PacketBuffer> Next;
	template <class _item_t> _item_t* Read(UINT Count=1)
		{
		assert(m_Read+Count*sizeof(_item_t)<=m_Size);
		auto p=(_item_t*)&m_Buffer[m_Read];
		m_Read+=Count*sizeof(_item_t);
		return p;
		}
	template <class _item_t> _item_t* Write(UINT Count=1)
		{
		assert(m_Written+Count*sizeof(_item_t)<=m_Size);
		auto p=(_item_t*)&m_Buffer[m_Written];
		m_Written+=Count*sizeof(_item_t);
		return p;
		}

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

protected:
	// Con-/Destructors
	friend Object;
	PacketBuffer(BYTE* Buffer, SIZE_T Size);

	// Common
	BYTE* m_Buffer;
	UINT m_Read;
	UINT m_Size;
	UINT m_Written;
};

}