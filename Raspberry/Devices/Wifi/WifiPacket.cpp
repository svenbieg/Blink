//================
// WifiPacket.cpp
//================

#include "WifiPacket.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==================
// Con-/Destructors
//==================

Handle<WifiPacket> WifiPacket::Create(UINT size)
{
UINT pkt_size=TypeHelper::AlignUp(sizeof(WIFI_HEADER)+size, 4U);
assert(pkt_size<=WIFI_PACKET_MAX);
auto pkt=Object::CreateEx<WifiPacket>(pkt_size);
auto header=pkt->Write<WIFI_HEADER>();
MemoryHelper::Zero(header, sizeof(WIFI_HEADER));
header->Length=pkt_size;
header->LengthChk=~pkt_size;
header->Sequence=s_SequenceId++;
header->DataOffset=sizeof(WIFI_HEADER);
return pkt;
}

Handle<WifiPacket> WifiPacket::Create(nullptr_t, UINT pkt_size)
{
assert(pkt_size<=WIFI_PACKET_MAX);
return Object::CreateEx<WifiPacket>(pkt_size);
}


//========
// Common
//========

BYTE WifiPacket::GetSequenceId()const
{
auto header=(WIFI_HEADER const*)m_Buffer;
return header->Sequence;
}

WifiPacketType WifiPacket::GetType()const
{
auto header=(WIFI_HEADER const*)m_Buffer;
return (WifiPacketType)BitHelper::Get(header->Flags, WIFI_HEADER_FLAGS_TYPE);
}

SIZE_T WifiPacket::WriteToStream(OutputStream* stream)
{
UINT size=GetSize();
if(!stream)
	return size;
auto buf=Begin();
return stream->Write(buf, size);
}


//==============
// Input-Stream
//==============

SIZE_T WifiPacket::Available()
{
return m_Size-m_Read;
}

SIZE_T WifiPacket::Read(VOID* buf, SIZE_T size)
{
SIZE_T copy=TypeHelper::Min(size, m_Size-m_Read);
if(buf)
	MemoryHelper::Copy(buf, &m_Buffer[m_Read], copy);
m_Read+=copy;
return copy;
}


//===============
// Output-Stream
//===============

VOID WifiPacket::Flush()
{
throw NotImplementedException();
}

SIZE_T WifiPacket::Write(VOID const* buf, SIZE_T size)
{
SIZE_T copy=TypeHelper::Min(size, m_Size-m_Written);
MemoryHelper::Copy(&m_Buffer[m_Written], buf, copy);
m_Written+=copy;
return copy;
}


//==========================
// Con-/Destructors Private
//==========================

WifiPacket::WifiPacket(BYTE* buf, SIZE_T size):
m_Buffer(buf),
m_Read(sizeof(WIFI_HEADER)),
m_Size(size),
m_Written(0)
{}


//================
// Common Private
//================

BYTE WifiPacket::s_SequenceId=0;

}}