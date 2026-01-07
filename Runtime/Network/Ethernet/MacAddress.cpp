//================
// MacAddress.cpp
//================

#include "MacAddress.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Ethernet {


//========
// Common
//========

MAC_ADDR MacAddress::From(BYTE a0, BYTE a1, BYTE a2, BYTE a3, BYTE a4, BYTE a5)
{
MAC_ADDR mac=0;
BYTE* ptr=(BYTE*)&mac;
ptr[0]=a0;
ptr[1]=a1;
ptr[2]=a2;
ptr[3]=a3;
ptr[4]=a4;
ptr[5]=a5;
return mac;
}

MAC_ADDR MacAddress::FromString(LPCSTR str)
{
BYTE b[MAC_ADDR_SIZE];
if(StringHelper::Scan(str, "%u:%u:%u:%u:%u:%u", &b[0], &b[1], &b[2], &b[3], &b[4], &b[5])!=MAC_ADDR_SIZE)
	return 0;
MAC_ADDR mac=0;
MemoryHelper::Copy(&mac, b, MAC_ADDR_SIZE);
return mac;
}

BOOL MacAddress::IsBroadcast(MAC_ADDR mac)
{
return mac==0xFFFFFFFFFFFF;
}

BOOL MacAddress::IsMulticast(MAC_ADDR mac)
{
auto ptr=(BYTE*)&mac;
return (ptr[0]==0x01)&&(ptr[1]==0x00)&&(ptr[2]==0x5E)&&!(ptr[3]&0x80);
}

Handle<String> MacAddress::ToString(MAC_ADDR mac)
{
auto ptr=(BYTE*)&mac;
BYTE a0=ptr[0];
BYTE a1=ptr[1];
BYTE a2=ptr[2];
BYTE a3=ptr[3];
BYTE a4=ptr[4];
BYTE a5=ptr[5];
return String::Create("%02x:%02x:%02x:%02x:%02x:%02x", a0, a1, a2, a3, a4, a5);
}

}}