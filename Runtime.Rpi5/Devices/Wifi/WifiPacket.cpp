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


//==========================
// Con-/Destructors Private
//==========================

WifiPacket::WifiPacket(BYTE* buf, SIZE_T size):
PacketBuffer(buf, size)
{}

}}