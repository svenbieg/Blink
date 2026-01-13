//=================
// WifiStation.cpp
//=================

#include "WifiStation.h"


//=======
// Using
//=======

#include "Devices/Wifi/Wifi.h"
#include "DebugHelper.h"

using namespace Devices::Wifi;


//===========
// Namespace
//===========

namespace Network {
	namespace Wifi {


//========
// Common
//========

VOID WifiStation::Connect()
{
if(m_ConnectTask)
	return;
m_ConnectTask=Task::Create(this, &WifiStation::ConnectTask, "wifi_connect");
m_ConnectTask->Then(this, [this]()
	{
	auto status=m_ConnectTask->GetStatus();
	m_ConnectTask=nullptr;
	});
}


//==========================
// Con-/Destructors Private
//==========================

WifiStation::WifiStation()
{}


//================
// Common Private
//================

VOID WifiStation::ConnectTask()
{
m_Device=WifiAdapter::Create();
m_Device->PacketReceived.Add(this, &WifiStation::OnWifiPacketReceived);
m_Device->Initialize();
Scan();
}

VOID WifiStation::OnWifiPacketReceived(WifiPacket* pkt)
{
UINT size=pkt->GetSize();
auto type=pkt->GetType();
switch(type)
	{
	case WifiPacketType::Event:
		{
		DebugHelper::Print("wifi_event (%u bytes)\n", size);
		break;
		}
	default:
		{
		DebugHelper::Print("wifi_pkt (%u bytes)\n", size);
		break;
		}
	}
}

VOID WifiStation::Scan()
{
//m_Device->Command(WifiCmd::SetPassiveScan, 0);
CHAR name[]="escan";
UINT name_size=sizeof(name);
UINT params_size=sizeof(WIFI_SCAN_PARAMS);
params_size+=sizeof(UINT);
params_size+=sizeof(WIFI_SCAN_CHANNELS);
UINT data_size=name_size+params_size;
UINT cmd_size=sizeof(WIFI_CMD)+data_size;
auto request=WifiPacket::Create(cmd_size);
auto cmd=request->Write<WIFI_CMD>();
MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
cmd->Command=WifiCmd::SetVariable;
cmd->Flags=WifiCmdFlags::Write;
cmd->Length=data_size;
request->Write(name, name_size);
auto params=request->Write<WIFI_SCAN_PARAMS>();
MemoryHelper::Zero(params, sizeof(WIFI_SCAN_PARAMS));
params->Version=WIFI_SCAN_VERSION;
params->Action=WifiScanAction::Start;
params->BssId={ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
params->BssType=WifiBssType::Any;
params->ScanType=WifiScanType::Passive;
params->ProbesCount=~0U;
params->ActiveTime=~0U;
params->PassiveTime=~0U;
params->HomeTime=~0U;
UINT channel_count=WIFI_SCAN_CHANNELS.size();
request->Write(&channel_count, sizeof(UINT));
request->Write(WIFI_SCAN_CHANNELS.data(), sizeof(WIFI_SCAN_CHANNELS));
auto response=m_Device->SendAndReceive(request);
cmd=response->Read<WIFI_CMD>();
response->Read(nullptr, name_size);
WIFI_SCAN_PARAMS header;
response->Read(&header, sizeof(WIFI_SCAN_PARAMS));
UINT copy=response->Available();
BYTE buf[512];
response->Read(buf, copy);
}

}}