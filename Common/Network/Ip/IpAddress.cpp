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

IP_ADDR IpAddress::Get()
{
IP_ADDR value=m_Value;
Reading(this, value);
return value;
}

Handle<String> IpAddress::ToString(LanguageCode lng)
{
return ToString(Get());
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

SIZE_T IpAddress::WriteToStream(OutputStream* Stream)
{
if(!Stream)
	return sizeof(IP_ADDR);
IP_ADDR value=Get();
return Stream->Write(&value, sizeof(IP_ADDR));
}


//==============
// Modification
//==============

BOOL IpAddress::FromString(Handle<String> str, BOOL notify)
{
IP_ADDR value;
if(!FromString(str, &value))
	return false;
return Set(value, notify);
}

BOOL IpAddress::FromString(Handle<String> value, IP_ADDR* ip_ptr)
{
if(!value)
	return false;
UINT a0=0;
UINT a1=0;
UINT a2=0;
UINT a3=0;
if(value->Scan("%u.%u.%u.%u", &a0, &a1, &a2, &a3)!=4)
	return false;
BYTE* ptr=(BYTE*)ip_ptr;
ptr[0]=(BYTE)a0;
ptr[1]=(BYTE)a1;
ptr[2]=(BYTE)a2;
ptr[3]=(BYTE)a3;
return true;
}

SIZE_T IpAddress::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return sizeof(IP_ADDR);
IP_ADDR value;
SIZE_T size=stream->Read(&value, sizeof(IP_ADDR));
if(size==sizeof(IP_ADDR))
	Set(value, notify);
return size;
}

BOOL IpAddress::Set(IP_ADDR value, BOOL notify)
{
if(m_Value==value)
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
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