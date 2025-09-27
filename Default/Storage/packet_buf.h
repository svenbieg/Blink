//==============
// packet_buf.h
//==============

#pragma once


//===========
// Namespace
//===========

namespace Storage {


//===============
// Packet-Buffer
//===============

class packet_buf
{
public:
	// Con-/Destructors
	packet_buf(UINT block_size);
	~packet_buf();

	// Common
	inline operator bool()const { return m_first!=nullptr; }
	inline SIZE_T available()const { return m_written-m_read; }
	VOID clear();
	inline VOID flush() { m_written=m_write; }
	SIZE_T read(VOID* buf, SIZE_T size);
	SIZE_T write(VOID const* buf, SIZE_T size);

private:
	// Block
	struct block_t
		{
		block_t* next;
		UINT size;
		BYTE buf[];
		};

	// Common
	UINT m_block_size;
	block_t* m_first;
	block_t* m_last;
	SIZE_T m_read;
	SIZE_T m_write;
	SIZE_T m_written;
};

}