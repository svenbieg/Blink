//==============
// RingBuffer.h
//==============

#pragma once


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
	// Con-/Destructors
	static Handle<RingBuffer> Create(SIZE_T Size);

	// Common
	SIZE_T Available();
	SIZE_T AvailableForWrite();
	SIZE_T BeginRead(BYTE** Buffer);
	SIZE_T BeginWrite(BYTE** Buffer);
	VOID Consume(SIZE_T Size);
	inline VOID Consume(BYTE* Value) { Read(Value, 1); }
	SIZE_T Read(VOID* Buffer, SIZE_T Size);
	inline VOID Write(BYTE Value) { Write(&Value, 1); }
	SIZE_T Write(VOID const* Buffer, SIZE_T Size);
	inline VOID Written(SIZE_T Size) { m_Head+=Size; }

private:
	// Con-/Destructors
	RingBuffer(BYTE* Buffer, SIZE_T Size);

	// Common
	BYTE* m_Buffer;
	SIZE_T m_Head;
	SIZE_T m_Size;
	SIZE_T m_Tail;
};

}