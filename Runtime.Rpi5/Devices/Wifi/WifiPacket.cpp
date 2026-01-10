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
UINT pkt_size=TypeHelper::AlignUp(sizeof(WIFI_HEADER)+size, 8U);
assert(pkt_size<=WIFI_PACKET_MAX);
auto pkt=Object::CreateEx<WifiPacket>(pkt_size);
auto header=pkt->Write<WIFI_HEADER>();
MemoryHelper::Zero(header, sizeof(WIFI_HEADER));
header->Length=pkt_size;
header->LengthChk=~pkt_size;
header->Sequence=s_SequenceId++;
header->DataOffset=sizeof(WIFI_HEADER);
pkt->m_Read=sizeof(WIFI_HEADER);
return pkt;
}

Handle<WifiPacket> WifiPacket::Create(WIFI_HEADER const& init)
{
UINT pkt_size=TypeHelper::AlignUp(init.Length, 8U);
assert(pkt_size<=WIFI_PACKET_MAX);
auto pkt=Object::CreateEx<WifiPacket>(pkt_size);
auto header=pkt->Write<WIFI_HEADER>();
MemoryHelper::Copy(header, &init, sizeof(WIFI_HEADER));
pkt->m_Read=sizeof(WIFI_HEADER);
return pkt;
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


//==========================
// Con-/Destructors Private
//==========================

WifiPacket::WifiPacket(BYTE* buf, SIZE_T size):
PacketBuffer(buf, size)
{}


//================
// Common Private
//================

BYTE WifiPacket::s_SequenceId=0;

}}