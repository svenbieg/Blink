//==============
// WifiPacket.h
//==============

#pragma once


//=======
// Using
//=======

#include "Collections/LinkedList.h"
#include "Devices/Wifi/Wifi.h"
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


//======
// Type
//======

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
	static Handle<WifiPacket> Create(UINT Payload);
	static Handle<WifiPacket> Create(nullptr_t, UINT Size);
	static Handle<WifiPacket> Create(WIFI_HEADER* Header);

	// Common
	inline BYTE* Begin()const noexcept { return m_Buffer; }
	BYTE* Data()const noexcept;
	BYTE GetSequenceId()const;
	UINT GetSize()const;
	WifiPacketType GetType()const;
	BOOL TryValidate();
	inline UINT Writable()const noexcept { return m_Size-m_Written; }
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