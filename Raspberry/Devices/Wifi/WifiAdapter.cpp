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


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//========
// Common
//========

VOID WifiAdapter::Command(WifiCommand id, UINT arg, WifiInterface interface)
{
WriteLock lock(m_Mutex);
Command(lock, id, &arg, sizeof(UINT), interface);
}

VOID WifiAdapter::Command(WifiCommand id, VOID* buf, SIZE_T size, WifiInterface interface)
{
WriteLock lock(m_Mutex);
Command(lock, id, buf, size, interface);
}

VOID WifiAdapter::EnableWifi()
{
WriteLock lock(m_Mutex);
if(FlagHelper::Get(m_Flags, WifiFlags::Enabled))
	return;
WifiFirmware::UploadRegulatory(this);
SetVariable(lock, "country", &WIFI_COUNTRY_DE, sizeof(WIFI_COUNTRY));
Task::Sleep(50);
WIFI_COUNTRY country;
GetVariable(lock, "country", &country, sizeof(WIFI_COUNTRY));
SetVariable(lock, "bus:txglom", 0);
SetVariable(lock, "apsta", 1);
Command(lock, WifiCommand::SetAntDiv, 0);
SetVariable(lock, "ampdu_ba_wsize", 8);
SetVariable(lock, "ampdu_mpdu", 4);
SetVariable(lock, "ampdu_rx_factor", 0);
Task::Sleep(150);
SetVariable(lock, "bsscfg:event_msgs", &WIFI_EVENT_MASK_STA, WIFI_EVENT_MASK_SIZE);
Task::Sleep(50);
//SetVariable(lock, "cur_etheraddr", &WIFI_MAC_ADDR, MAC_ADDR_SIZE);
//SetVariable(lock, "assoc_listen", 10);
//SetVariable(lock, "bcn_timeout", 10);
//SetVariable(lock, "assoc_retry_max", 3);
//Command(lock, WifiCommand::SetScanChannelTime, 40);
//Command(lock, WifiCommand::SetScanUnassocTime, 40);
//Command(lock, WifiCommand::SetScanPassiveTime, 130);
//SetVariable(lock, "roam_off", 1);
//Command(lock, WifiCommand::SetPromisc, 0);
Command(lock, WifiCommand::Up, 1);
Task::Sleep(50);
FlagHelper::Set(m_Flags, WifiFlags::Enabled);
}

VOID WifiAdapter::GetVariable(LPCSTR name, VOID* buf, UINT size, WifiInterface interface)
{
WriteLock lock(m_Mutex);
GetVariable(lock, name, buf, size, interface);
}

VOID WifiAdapter::SetVariable(LPCSTR name, UINT value, WifiInterface interface)
{
WriteLock lock(m_Mutex);
SetVariable(lock, name, &value, sizeof(UINT), interface);
}

VOID WifiAdapter::SetVariable(LPCSTR name, VOID const* buf, UINT size, WifiInterface interface)
{
WriteLock lock(m_Mutex);
SetVariable(lock, name, buf, size, interface);
}


//==========================
// Con-/Destructors Private
//==========================

WifiAdapter::WifiAdapter():
m_Flags(WifiFlags::None),
m_Request(nullptr),
m_RequestId(0)
{}


//================
// Common Private
//================

VOID WifiAdapter::Command(WriteLock const& lock, WifiCommand id, UINT arg, WifiInterface interface)
{
Command(lock, id, &arg, sizeof(UINT), interface);
}

VOID WifiAdapter::Command(WriteLock const& lock, WifiCommand id, VOID* buf, SIZE_T size, WifiInterface interface)
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

VOID WifiAdapter::GetVariable(WriteLock const& lock, LPCSTR name, VOID* buf, UINT size, WifiInterface interface)
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
request->Flush();
auto response=Transfer(request);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
UINT available=response->Available();
UINT copy=TypeHelper::Min(size, available);
response->Read(buf, copy);
}

VOID WifiAdapter::OnDataReceived(WifiPacket* pkt)
{
}

VOID WifiAdapter::OnEventReceived(WIFI_EVENT_MESSAGE* msg)
{
[[maybe_unused]] Status status=Status::Success;
switch(msg->EventType)
	{
	case WifiEventType::Auth:
		{
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
		if(msg->Status!=0)
			status=Status::ConnectionFailed;
		break;
		}
	default:
		{
		throw NotImplementedException();
		}
	}
}

VOID WifiAdapter::OnResponseReceived(WifiPacket* pkt)
{
if(!m_Request)
	return;
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
		OnDataReceived(pkt);
		break;
		}
	case WifiPacketType::Event:
		{
		auto msg=ParseEvent(pkt);
		if(!msg)
			break;
		OnEventReceived(msg);
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
if(frame->EtherType!=ETHERTYPE_BRCM)
	return nullptr;
auto oui=frame->Broadcom.Oui;
UINT bcm_id=TypeHelper::MakeLong(oui[0], oui[1], oui[2], 0);
if(bcm_id!=0x181000)
	return nullptr;
return &frame->Event;
}

VOID WifiAdapter::SetVariable(WriteLock const& lock, LPCSTR name, UINT value, WifiInterface interface)
{
SetVariable(lock, name, &value, sizeof(UINT), interface);
}

VOID WifiAdapter::SetVariable(WriteLock const& lock, LPCSTR name, VOID const* buf, UINT size, WifiInterface interface)
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
request->Flush();
auto response=Transfer(request);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
}

Handle<WifiPacket> WifiAdapter::Transfer(WifiPacket* request)
{
assert(!m_Request);
m_Request=request;
WriteLock lock(WifiSdio::m_Mutex);
try
	{
	WritePacket(lock, request);
	m_ResponseReceived.Wait(lock, 1000);
	}
catch(Exception e)
	{
	m_Request=nullptr;
	throw;
	}
return request->m_Response;
}

}}