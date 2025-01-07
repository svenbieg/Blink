//===============
// IpAddress.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "IpAddress.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Ip {


//========
// Access
//========

Handle<String> IpAddress::ToString()
{
IP_ADDR ip=Get();
return ToString(ip);
}

Handle<String> IpAddress::ToString(IP_ADDR ip)
{
BYTE* ptr=(BYTE*)&ip;
UINT a0=ptr[0];
UINT a1=ptr[1];
UINT a2=ptr[2];
UINT a3=ptr[3];
return String::Create("%u.%u.%u.%u", a0, a1, a2, a3);
}


//==============
// Modification
//==============

BOOL IpAddress::FromString(Handle<String> value, BOOL notify)
{
IP_ADDR ip=0;
if(FromString(value, &ip))
	{
	TypedVariable::Set(ip, notify);
	return true;
	}
return false;
}

BOOL IpAddress::FromString(Handle<String> value, IP_ADDR* ip_ptr)
{
if(!value)
	return false;
UINT a0=0;
UINT a1=0;
UINT a2=0;
UINT a3=0;
if(value->Scan("%u.%u.%u.%u", &a0, &a1, &a2, &a3)==4)
	{
	BYTE* ptr=(BYTE*)ip_ptr;
	ptr[0]=(BYTE)a0;
	ptr[1]=(BYTE)a1;
	ptr[2]=(BYTE)a2;
	ptr[3]=(BYTE)a3;
	return true;
	}
return false;
}


//========
// Common
//========

IP_ADDR IpAddress::From(BYTE a0, BYTE a1, BYTE a2, BYTE a3)
{
IP_ADDR ip=0;
BYTE* ptr=(BYTE*)&ip;
ptr[0]=a0;
ptr[1]=a1;
ptr[2]=a2;
ptr[3]=a3;
return ip;
}

}}