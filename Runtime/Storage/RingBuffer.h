//==============
// RingBuffer.h
//==============

#pragma once


//=======
// Using
//=======

#include "Handle.h"


//===========
// Namespace
//===========

namespace Storage {


//=============
// Ring-Buffer
//=============

class RingBuffer: public Object
{
public:
	// Friends
	friend Object;

	// Con-/Destructors
	static inline Handle<RingBuffer> Create(SIZE_T Size, SIZE_T Align=sizeof(SIZE_T)) { return Object::CreateEx<RingBuffer>(Size, Align); }

	// Common
	SIZE_T Available()noexcept;
	SIZE_T AvailableForWrite()noexcept;
	SIZE_T BeginRead(BYTE** Buffer)noexcept;
	SIZE_T BeginWrite(BYTE** Buffer)noexcept;
	VOID Consumed(SIZE_T Size)noexcept;
	inline BOOL Read(BYTE* Value)noexcept { return Read(Value, 1)==1; }
	SIZE_T Read(VOID* Buffer, SIZE_T Size)noexcept;
	inline BOOL Write(BYTE Value)noexcept { return Write(&Value, 1)==1; }
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)noexcept;
	inline VOID Written(SIZE_T Size)noexcept { m_Head+=Size; }

private:
	// Con-/Destructors
	RingBuffer(VOID* Buffer, SIZE_T Size)noexcept;

	// Common
	BYTE* m_Buffer;
	SIZE_T m_Head;
	SIZE_T m_Size;
	SIZE_T m_Tail;
};

}