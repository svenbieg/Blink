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
using namespace Network::Ethernet;

extern BYTE wifi_config[];
extern UINT wifi_config_size;
extern BYTE wifi_firmware[];
extern UINT wifi_firmware_size;
extern BYTE wifi_clm[];
extern UINT wifi_clm_size;


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==========
// Settings
//==========

const UINT WIFI_TIMEOUT=100;


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

VOID WifiAdapter::Command(WifiCommand id, VOID* buf, SIZE_T size, WifiInterface interface)
{
WriteLock lock(m_Mutex);
Command(AccessMode::Locked, id, buf, size, interface);
}

VOID WifiAdapter::Enable()
{
WriteLock lock(m_Mutex);
if(FlagHelper::Get(m_Flags, WifiFlags::SdioEnabled))
	return;
EnableSdio();
m_Ready.Wait(2000);
FlagHelper::Set(m_Flags, WifiFlags::SdioEnabled);
}

VOID WifiAdapter::EnableWifi()
{
WriteLock lock(m_Mutex);
if(FlagHelper::Get(m_Flags, WifiFlags::WifiEnabled))
	return;
if(!FlagHelper::Get(m_Flags, WifiFlags::SdioEnabled))
	{
	EnableSdio();
	m_Ready.Wait(2000);
	FlagHelper::Set(m_Flags, WifiFlags::SdioEnabled);
	}
UploadRegulatory();
WIFI_COUNTRY country;
SetVariable(AccessMode::Locked, "country", &WIFI_COUNTRY_DE, sizeof(WIFI_COUNTRY));
Task::Sleep(50);
GetVariable(AccessMode::Locked, "country", &country, sizeof(WIFI_COUNTRY));
SetVariable(AccessMode::Locked, "bus:txglom", 0);
SetVariable(AccessMode::Locked, "apsta", 1);
Command(AccessMode::Locked, WifiCommand::SetAntDiv, 0);
SetVariable(AccessMode::Locked, "ampdu_ba_wsize", 8);
SetVariable(AccessMode::Locked, "ampdu_mpdu", 4);
SetVariable(AccessMode::Locked, "ampdu_rx_factor", 0);
Task::Sleep(150);
SetVariable(AccessMode::Locked, "bsscfg:event_msgs", &WIFI_EVENT_MASK_STA, WIFI_EVENT_MASK_SIZE);
Task::Sleep(50);
//SetVariable(AccessMode::Locked, "cur_etheraddr", &WIFI_MAC_ADDR, MAC_ADDR_SIZE);
//SetVariable(AccessMode::Locked, "assoc_listen", 10);
//SetVariable(AccessMode::Locked, "bcn_timeout", 10);
//SetVariable(AccessMode::Locked, "assoc_retry_max", 3);
//Command(AccessMode::Locked, WifiCommand::SetScanChannelTime, 40);
//Command(AccessMode::Locked, WifiCommand::SetScanUnassocTime, 40);
//Command(AccessMode::Locked, WifiCommand::SetScanPassiveTime, 130);
//SetVariable(AccessMode::Locked, "roam_off", 1);
//Command(AccessMode::Locked, WifiCommand::SetPromisc, 0);
Command(AccessMode::Locked, WifiCommand::Up, 1);
Task::Sleep(50);
FlagHelper::Set(m_Flags, WifiFlags::WifiEnabled);
}

VOID WifiAdapter::GetVariable(LPCSTR name, VOID* buf, UINT size, WifiInterface interface)
{
WriteLock lock(m_Mutex);
GetVariable(AccessMode::Locked, name, buf, size, interface);
}

VOID WifiAdapter::SetVariable(LPCSTR name, VOID const* buf, UINT size, WifiInterface interface)
{
WriteLock lock(m_Mutex);
SetVariable(AccessMode::Locked, name, buf, size, interface);
}


//============================
// Con-/Destructors Protected
//============================

WifiAdapter::WifiAdapter():
m_Flags(WifiFlags::None),
m_Request(nullptr),
m_RequestId(0)
{
wifi_config_size=InitializeConfiguration(wifi_config, wifi_config_size);
}


//================
// Common Private
//================

VOID WifiAdapter::Command(AccessMode access, WifiCommand id, VOID* buf, SIZE_T size, WifiInterface interface)
{
UINT cmd_size=sizeof(WIFI_CMD)+size;
auto request=WifiPacket::Create(cmd_size);
auto cmd=request->Write<WIFI_CMD>();
MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
cmd->Command=id;
cmd->Id=m_RequestId++;
cmd->Flags=WifiCmdFlags::Write;
FlagHelper::Set(cmd->Flags, (UINT)interface<<WIFI_CMD_INTERFACE_SHIFT);
cmd->Length=size;
request->Write(buf, size);
auto response=Transfer(request);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
}

VOID WifiAdapter::Disconnect()
{
throw NotImplementedException();
}

VOID WifiAdapter::GetVariable(AccessMode access, LPCSTR name, VOID* buf, UINT size, WifiInterface interface)
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
FlagHelper::Set(cmd->Flags, (UINT)interface<<WIFI_CMD_INTERFACE_SHIFT);
cmd->Length=data_size;
request->Write(name, name_size);
auto response=Transfer(request);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
UINT available=response->Available();
UINT copy=TypeHelper::Min(size, available);
response->Read(buf, copy);
}

UINT WifiAdapter::InitializeConfiguration(BYTE* buf, UINT size)
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

VOID WifiAdapter::OnResponseReceived(WifiPacket* pkt)
{
assert(m_Request);
auto cmd=(WIFI_CMD*)m_Request->Data();
WORD request_id=cmd->Id;
cmd=(WIFI_CMD*)pkt->Data();
WORD response_id=cmd->Id;
if(request_id!=response_id)
	return;
m_Request->m_Response=pkt;
m_Request=nullptr;
m_ResponseReceived.Trigger();
}

VOID WifiAdapter::OnPacketReceived(Handle<WifiPacket> pkt)
{
auto type=pkt->GetType();
switch(type)
	{
	case WifiPacketType::Data:
		{
		break;
		}
	case WifiPacketType::Event:
		{
		auto msg=ParseEvent(pkt);
		if(!msg)
			{
			m_Ready.Trigger();
			break;
			}
		switch(msg->EventType)
			{
			case WifiEventType::Auth:
				{
				Status status=Status::Success;
				switch(msg->Status)
					{
					case 0:
					case 2:
					case 5:
					case 6:
						break;
					default:
						{
						status=Status::AccessDenied;
						break;
						}
					}
				break;
				}
			case WifiEventType::Deauth:
			case WifiEventType::DeauthInd:
			case WifiEventType::Disassoc:
				{
				Disconnect();
				break;
				}
			case WifiEventType::Link:
				{
				if(msg->Status==0)
					{
					if(msg->InterfaceIndex==WIFI_STA)
						{
						break;
						}
					}
				break;
				}
			case WifiEventType::SetSsid:
				{
				Status status=Status::Success;
				if(msg->Status!=0)
					status=Status::ConnectionFailed;
				break;
				}
			}
		break;
		}
	case WifiPacketType::Response:
		{
		OnResponseReceived(pkt);
		break;
		}
	}
}

WIFI_EVENT_MESSAGE* WifiAdapter::ParseEvent(WifiPacket* pkt)
{
auto buf=pkt->Begin();
auto wifi_header=(WIFI_HEADER*)buf;
UINT pos=wifi_header->DataOffset;
auto bdc_header=(WIFI_BDC_HEADER*)&buf[pos];
pos+=sizeof(WIFI_BDC_HEADER);
UINT frame_offset=pos+bdc_header->DataOffset*4;
UINT size=pkt->GetSize();
if(frame_offset>=size)
	return nullptr;
pos=frame_offset;
auto frame=(WIFI_EVENT_FRAME*)&buf[pos];
if(frame->Ethernet.Protocol!=EthernetProtocol::Brcm)
	return nullptr;
auto oui=frame->Broadcom.Oui;
UINT bcm_id=TypeHelper::MakeLong(oui[0], oui[1], oui[2], 0);
if(bcm_id!=0x181000)
	return nullptr;
return &frame->Event;
}

VOID WifiAdapter::SetVariable(AccessMode access, LPCSTR name, VOID const* buf, UINT size, WifiInterface interface)
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
FlagHelper::Set(cmd->Flags, (UINT)interface<<WIFI_CMD_INTERFACE_SHIFT);
cmd->Length=data_size;
request->Write(name, name_size);
request->Write(buf, size);
auto response=Transfer(request);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
}

Handle<WifiPacket> WifiAdapter::Transfer(WifiPacket* request)
{
assert(!m_Request);
m_Request=request;
WritePacket(request);
m_ResponseReceived.Wait(WIFI_TIMEOUT);
return request->m_Response;
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
	pkt->Write(&wifi_clm[pos], copy);
	auto response=Transfer(pkt);
	cmd=response->Read<WIFI_CMD>();
	if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
		throw InvalidArgumentException();
	BitHelper::Clear(clm_flags, CLMF_FIRST);
	pos+=copy;
	}
}

}}