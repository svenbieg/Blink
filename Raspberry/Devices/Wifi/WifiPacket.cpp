//================
// WifiPacket.cpp
//================

#include "WifiPacket.h"


//=======
// Using
//=======

#include "Devices/Wifi/Wifi.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==================
// Con-/Destructors
//==================

Handle<WifiPacket> WifiPacket::Create(UINT payload)
{
assert(payload>0);
UINT pkt_len=sizeof(WIFI_HEADER)+payload;
pkt_len=TypeHelper::AlignUp(pkt_len, sizeof(UINT));
auto pkt=Object::CreateEx<WifiPacket>(pkt_len);
auto header=pkt->Write<WIFI_HEADER>();
MemoryHelper::Zero(header, sizeof(WIFI_HEADER));
header->Length=pkt_len;
header->LengthChk=~pkt_len;
header->Sequence=s_SequenceId++;
header->DataOffset=sizeof(WIFI_HEADER);
return pkt;
}

Handle<WifiPacket> WifiPacket::Create(nullptr_t, UINT pkt_size)
{
assert(pkt_size>0);
assert(pkt_size%sizeof(UINT)==0);
assert(pkt_size<=WIFI_PACKET_MAX);
return Object::CreateEx<WifiPacket>(pkt_size);
}

Handle<WifiPacket> WifiPacket::Create(WIFI_HEADER* header)
{
UINT pkt_len=TypeHelper::AlignUp(header->Length, sizeof(UINT));
assert(pkt_len!=0);
assert(pkt_len<=WIFI_PACKET_MAX);
auto pkt=Object::CreateEx<WifiPacket>(pkt_len);
auto pkt_header=(WIFI_HEADER*)pkt->Begin();
*pkt_header=*header;
pkt->m_Written=sizeof(WIFI_HEADER);
pkt->m_Read=pkt_header->DataOffset;
return pkt;
}


//========
// Common
//========

BYTE* WifiPacket::Data()const noexcept
{
auto header=(WIFI_HEADER const*)m_Buffer;
UINT pos=header->DataOffset;
return &m_Buffer[pos];
}

BYTE WifiPacket::GetSequenceId()const
{
auto header=(WIFI_HEADER const*)m_Buffer;
return header->Sequence;
}

UINT WifiPacket::GetSize()const
{
auto header=(WIFI_HEADER const*)m_Buffer;
return header->Length;
}

WifiPacketType WifiPacket::GetType()const
{
auto header=(WIFI_HEADER const*)m_Buffer;
return (WifiPacketType)BitHelper::Get(header->Flags, WIFI_HEADER_FLAGS_TYPE);
}

BOOL WifiPacket::TryValidate()
{
auto header=(WIFI_HEADER const*)m_Buffer;
WORD len=header->Length;
WORD len_chk=header->LengthChk;
if(len_chk!=(len^0xFFFF))
	return false;
m_Read=header->DataOffset;
return true;
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
UINT pkt_size=m_Written;
auto header=(WIFI_HEADER*)m_Buffer;
header->Length=pkt_size;
header->LengthChk=~pkt_size;
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
m_Read(0),
m_Size(size),
m_Written(0)
{}


//================
// Common Private
//================

BYTE WifiPacket::s_SequenceId=0;

}}