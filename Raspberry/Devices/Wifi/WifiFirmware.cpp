//==================
// WifiFirmware.cpp
//==================

#include "WifiFirmware.h"


//=======
// Using
//=======

#include "Devices/Wifi/WifiAdapter.h"

extern BYTE wifi_clm[];
extern UINT wifi_clm_size;
extern UINT wifi_config[];
extern UINT wifi_config_size;
extern UINT wifi_firmware[];
extern UINT wifi_firmware_size;


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


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

WIFI_FIRMWARE const& WifiFirmware::Get()
{
if(s_Firmware.ConfigSize)
	return s_Firmware;
auto src=(LPSTR)wifi_config;
UINT size=wifi_config_size;
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
wifi_config_size=dst_pos;
s_Firmware.Config=wifi_config;
s_Firmware.ConfigSize=wifi_config_size;
s_Firmware.Image=wifi_firmware;
s_Firmware.ImageSize=wifi_firmware_size;
return s_Firmware;
}

VOID WifiFirmware::UploadRegulatory(WifiAdapter* wifi)
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
	cmd->Command=WifiCommand::SetVariable;
	cmd->Id=wifi->m_RequestId++;
	cmd->Flags=WifiCmdFlags::Write;
	cmd->Length=name_size+clm_size;
	cmd->Status=0;
	pkt->Write(name, name_size);
	auto clm=pkt->Write<CLM_HEADER>();
	clm->Flags=clm_flags;
	clm->Type=CLM_TYPE;
	clm->Size=copy;
	clm->Crc=0;
	pkt->Write(&wifi_clm[pos], copy);
	auto response=wifi->Transfer(pkt);
	cmd=response->Read<WIFI_CMD>();
	if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
		throw InvalidArgumentException();
	BitHelper::Clear(clm_flags, CLMF_FIRST);
	pos+=copy;
	}
}


//================
// Common Private
//================

WIFI_FIRMWARE WifiFirmware::s_Firmware={ 0 };

}}