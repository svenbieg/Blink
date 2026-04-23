//=================
// WifiAdapter.cpp
//=================

#include "WifiAdapter.h"


//=======
// Using
//=======

#include "Concurrency/ServiceTask.h"
#include "Devices/System/Cpu.h"
#include "Devices/Wifi/WifiConfig.h"

using namespace Concurrency;
using namespace Devices::System;

extern "C" UINT wifi_config;
extern "C" UINT wifi_config_size;
extern "C" UINT wifi_firmware;
extern "C" UINT wifi_firmware_size;
extern "C" UINT wifi_clm;
extern "C" UINT wifi_clm_size;


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==========
// Settings
//==========

const UINT WIFI_TIMEOUT=1000;


//============
// Regulatory
//============

typedef struct
{
WORD Flags;
WORD Type;
UINT Size;
UINT Crc;
}CLM_HEADER;

const WORD CLM_TYPE			=2;
const UINT CLM_DATA_MAX		=1024;

const WORD CLMF_CLM			=(1<<12);
const WORD CLMF_LAST		=(1<<2);
const WORD CLMF_FIRST		=(1<<1);


//========
// Common
//========

VOID WifiAdapter::Command(WifiCommand id, UINT arg)
{
UINT cmd_size=sizeof(WIFI_CMD)+sizeof(UINT);
auto request=WifiPacket::Create(cmd_size);
auto cmd=request->Write<WIFI_CMD>();
MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
cmd->Command=id;
cmd->Id=m_RequestId++;
cmd->Flags=WifiCmdFlags::Write;
cmd->Length=sizeof(UINT);
request->Write(&arg, sizeof(UINT));
auto response=SendAndReceive(request);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
}

VOID WifiAdapter::GetVariable(LPCSTR name, VOID* buf, UINT size)
{
UINT name_size=StringHelper::Length(name)+1;
UINT data_size=TypeHelper::Max(name_size, size);
UINT cmd_size=sizeof(WIFI_CMD)+data_size;
auto request=WifiPacket::Create(cmd_size);
auto cmd=request->Write<WIFI_CMD>();
MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
cmd->Command=WifiCommand::GetVariable;
cmd->Id=m_RequestId++;
cmd->Flags=WifiCmdFlags::Read;
cmd->Length=data_size;
request->Write(name, name_size);
auto response=SendAndReceive(request);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
UINT available=response->Available();
UINT copy=TypeHelper::Min(size, available);
response->Read(buf, copy);
}

Handle<WifiPacket> WifiAdapter::SendAndReceive(Handle<WifiPacket> request)
{
WriteLock lock(m_Mutex);
if(m_Request)
	{
	m_Requests.Append(request);
	}
else
	{
	m_Request=request;
	m_Request->WriteToStream(m_Sdio);
	}
m_ResponseReceived.Wait(lock, WIFI_TIMEOUT);
m_Request=m_Requests.RemoveFirst();
if(m_Request)
	m_Request->WriteToStream(m_Sdio);
return request->m_Response;
}

VOID WifiAdapter::SetVariable(LPCSTR name, VOID const* buf, UINT size)
{
UINT name_size=StringHelper::Length(name)+1;
UINT data_size=name_size+size;
UINT cmd_size=sizeof(WIFI_CMD)+data_size;
auto request=WifiPacket::Create(cmd_size);
auto cmd=request->Write<WIFI_CMD>();
MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
cmd->Command=WifiCommand::SetVariable;
cmd->Id=m_RequestId++;
cmd->Flags=WifiCmdFlags::Write;
cmd->Length=data_size;
request->Write(name, name_size);
request->Write(buf, size);
auto response=SendAndReceive(request);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
}


//============================
// Con-/Destructors Protected
//============================

WifiAdapter::WifiAdapter():
m_Request(nullptr),
m_RequestId(0)
{
wifi_config_size=InitializeConfiguration(&wifi_config, wifi_config_size);
m_Sdio=WifiSdio::Create();
m_Sdio->PacketReceived.Add(this, &WifiAdapter::OnPacketReceived);
}


//==================
// Common Protected
//==================

UINT WifiAdapter::InitializeConfiguration(UINT* buf, UINT size)
{
auto src=(LPSTR)buf;
auto dst=src;
UINT dst_pos=0;
BOOL comment=false;
BOOL zero=true;
for(UINT src_pos=0; src_pos<size; src_pos++)
	{
	CHAR c=src[src_pos];
	switch(c)
		{
		case '\0':
		case '\r':
			{
			continue;
			}
		case '\n':
			{
			comment=false;
			if(!zero)
				dst[dst_pos++]=0;
			zero=true;
			continue;
			}
		case '#':
			{
			comment=true;
			continue;
			}
		default:
			{
			if(comment)
				break;
			dst[dst_pos++]=c;
			zero=false;
			continue;
			}
		}
	}
if(dst[dst_pos-1]!=0)
	dst[dst_pos++]=0;
while(dst_pos%4)
	dst[dst_pos++]=0;
UINT token=dst_pos/4;
token=(~token<<16)|(token&0xFFFF);
auto valid=(UINT*)&dst[dst_pos];
*valid=token;
dst_pos+=4;
return dst_pos;
}

VOID WifiAdapter::OnPacketReceived(WifiPacket* pkt)
{
auto type=pkt->GetType();
switch(type)
	{
	case WifiPacketType::Event:
		{
		Ready.Trigger();
		break;
		}
	case WifiPacketType::Response:
		{
		WriteLock lock(m_Mutex);
		m_Request->m_Response=pkt;
		m_ResponseReceived.Trigger();
		break;
		}
	default:
		{
		PacketReceived(this, pkt);
		break;
		}
	}
}

VOID WifiAdapter::UploadRegulatory()
{
CHAR name[]="clmload";
UINT name_size=sizeof(name);
WORD clm_flags=CLMF_CLM|CLMF_FIRST;
WORD pos=0;
while(pos<wifi_clm_size)
	{
	WORD copy=TypeHelper::Min((WORD)(wifi_clm_size-pos), CLM_DATA_MAX);
	if(pos+copy==wifi_clm_size)
		BitHelper::Set(clm_flags, CLMF_LAST);
	WORD clm_size=sizeof(CLM_HEADER)+copy;
	WORD cmd_size=sizeof(WIFI_CMD)+name_size+clm_size;
	auto pkt=WifiPacket::Create(cmd_size);
	auto cmd=pkt->Write<WIFI_CMD>();
	MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
	cmd->Command=WifiCommand::SetVariable;
	cmd->Id=m_RequestId++;
	cmd->Flags=WifiCmdFlags::Write;
	cmd->Length=name_size+clm_size;
	pkt->Write(name, name_size);
	auto clm=pkt->Write<CLM_HEADER>();
	MemoryHelper::Zero(clm, sizeof(CLM_HEADER));
	clm->Flags=clm_flags;
	clm->Size=copy;
	clm->Type=CLM_TYPE;
	pkt->Write(&clm[pos], copy);
	auto response=SendAndReceive(pkt);
	cmd=response->Read<WIFI_CMD>();
	if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
		throw InvalidArgumentException();
	BitHelper::Clear(clm_flags, CLMF_FIRST);
	pos+=copy;
	}
}

}}