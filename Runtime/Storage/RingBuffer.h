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
	static inline Handle<RingBuffer> Create(SIZE_T Size, SIZE_T Align=0) { return Object::CreateEx<RingBuffer>(Size, Align); }

	// Common
	SIZE_T Available();
	SIZE_T AvailableForWrite();
	SIZE_T BeginRead(BYTE** Buffer);
	SIZE_T BeginWrite(BYTE** Buffer);
	VOID Consumed(SIZE_T Size);
	inline BOOL Read(BYTE* Value) { return Read(Value, 1)==1; }
	SIZE_T Read(VOID* Buffer, SIZE_T Size);
	inline BOOL Write(BYTE Value) { return Write(&Value, 1)==1; }
	SIZE_T Write(VOID const* Buffer, SIZE_T Size);
	inline VOID Written(SIZE_T Size) { m_Head+=Size; }

private:
	// Con-/Destructors
	RingBuffer(VOID* Buffer, SIZE_T Size);

	// Common
	BYTE* m_Buffer;
	SIZE_T m_Head;
	SIZE_T m_Size;
	SIZE_T m_Tail;
};

}