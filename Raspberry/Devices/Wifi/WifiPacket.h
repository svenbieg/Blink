//==============
// WifiPacket.h
//==============

#pragma once


//=======
// Using
//=======

#include "Collections/LinkedList.h"
#include "Storage/Streams/RandomAccessStream.h"
#include "BitHelper.h"
#include "Handle.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==========
// Settings
//==========

static const UINT WIFI_PACKET_MAX=2048;


//======================
// Forward-Declarations
//======================

class WifiAdapter;


//========
// Header
//========

typedef struct alignas(UINT)
{
WORD Length;
WORD LengthChk;
BYTE Sequence;
BYTE Flags;
BYTE NextLength;
BYTE DataOffset;
BYTE FlowControl;
BYTE Window;
WORD Reserved;
}WIFI_HEADER;

static const BITS8 WIFI_HEADER_FLAGS_TYPE={ 0xF, 0 };

enum class WifiPacketType
{
Response=0,
Event=1,
Data=2
};


//=============
// Wifi-Packet
//=============

class WifiPacket: public Object, public Storage::Streams::RandomAccessStream
{
public:
	// Friends
	friend WifiAdapter;

	// Con-/Destructors
	static Handle<WifiPacket> Create(UINT Size);
	static Handle<WifiPacket> Create(WIFI_HEADER const& Header);
	static Handle<WifiPacket> ReadFromStream(InputStream* Stream);

	// Common
	inline UINT AvailableForWrite()const noexcept { return m_Size-m_Written; }
	inline BYTE* Begin()const noexcept { return m_Buffer; }
	BYTE GetSequenceId()const;
	inline UINT GetSize()const noexcept { return m_Size; }
	WifiPacketType GetType()const;
	SIZE_T WriteToStream(OutputStream* Stream);

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;
	template <class _item_t> _item_t* Read(UINT Count=1)noexcept
		{
		auto p=(_item_t*)&m_Buffer[m_Read];
		m_Read+=Count*sizeof(_item_t);
		assert(m_Read<=m_Size);
		return p;
		}

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;
	template <class _item_t> _item_t* Write(UINT Count=1)noexcept
		{
		auto p=(_item_t*)&m_Buffer[m_Written];
		m_Written+=Count*sizeof(_item_t);
		assert(m_Written<=m_Size);
		return p;
		}


private:
	// Using
	using WifiPacketLink=Collections::Link<WifiPacket>;

	// Con-/Destructors
	friend Object;
	WifiPacket(BYTE* Buffer, SIZE_T Size);

	// Common
	BYTE* m_Buffer;
	WifiPacketLink m_Link;
	UINT m_Read;
	Handle<WifiPacket> m_Response;
	UINT m_Size;
	UINT m_Written;
	static BYTE s_SequenceId;
};

}}