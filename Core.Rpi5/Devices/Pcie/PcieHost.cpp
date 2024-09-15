//==============
// PcieHost.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Core/Application.h"
#include "Devices/System/Peripherals.h"
#include "Devices/System/System.h"
#include "PcieHost.h"
#include "PcieRegisters.h"

using namespace Core;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Pcie {


//==========
// Settings
//==========

constexpr UINT PCIE_HOST_CLASS=0x060400;
constexpr WORD PCIE_GEN=2;

constexpr SIZE_T RBAR1_OFFSET=0xFFFFFFF000; // MSI - 0xFFFFFFF000
constexpr SIZE_T RBAR1_SIZE=0x1000;

constexpr SIZE_T RBAR2_OFFSET=AXI_IO_BASE; // AXI - 0x1000000000
constexpr SIZE_T RBAR2_SIZE=0x100000000;

constexpr SIZE_T OUTWND_OFFSET=RP1_IO_BASE; // RP1 - 0x1F00000000
constexpr SIZE_T OUTWND_SIZE=0xFFFFFFFC;


//========
// Common
//========

BOOL PcieHost::EnableDevice(PCIE_DEVICE_INFO const& device)
{
if(!m_Initialized)
	return false;
auto pcie=(PCIE_REGS*)MapConfiguration(1, device.Slot, device.Function);
if(Bits::Get(pcie->CLASSREV, CLASSREV_CLASS)!=device.Id)
	return false;
if(Bits::Get(pcie->HEADER_TYPE)!=HEADER_TYPE_NORMAL)
	return false;
Bits::Write(pcie->CACHE_LINE_SIZE, CACHE_LINE_SIZE/4);
Bits::Write(pcie->BASE_LO, BASE_MEM_TYPE_64);
Bits::Write(pcie->BASE_HI, 0);
Bits::Write(pcie->IRQ_PIN, 1);
Bits::Write(pcie->CMD, CMD_MEMORY|CMD_MASTER|CMD_PARITY|CMD_SERR);
SleepMicroseconds(10);
return true;
}

Handle<PcieHost> PcieHost::Open()
{
if(s_Default)
	return s_Default;
if(!Application::Current)
	return nullptr;
s_Default=new PcieHost();
return s_Default;
}


//==========================
// Con-/Destructors Private
//==========================

PcieHost::PcieHost():
m_Initialized(false)
{
auto task=CreateTask(this, &PcieHost::Initialize);
task->Then([this, task]()
	{
	auto status=task->GetStatus();
	if(Failed(status))
		return;
	m_Initialized=true;
	});
}


//================
// Common Private
//================

UINT PcieHost::EncodeBarSize(UINT64 size)
{
UINT bits=GetBitCount(size);
if(bits<12)
	return 0;
if(bits<16)
	return (bits-12)+0x1C; // 4kB..32kB
if(bits<38)
	return bits-15; // 64kB..32GB
return 0;
}

UINT PcieHost::GetBitCount(UINT64 value)
{
UINT bits=0;
while(value>0)
	{
	value>>=1;
	bits++;
	}
return bits;
}

VOID PcieHost::Initialize()
{
Reset();
auto pcie=(PCIE_HOST_REGS*)AXI_PCIE_HOST_BASE;
Bits::Clear(pcie->HARD_DEBUG, HARD_DEBUG_SERDES_IDDQ);
SleepMicroseconds(100);
constexpr BYTE regs[]={ 0x16, 0x17, 0x18, 0x19, 0x1B, 0x1C, 0x1E };
constexpr WORD data[]={ 0x50B9, 0xBDA1, 0x0094, 0x97B4, 0x5030, 0x5030, 0x0007 };
MdioWrite(0, MDIO_REG_SET_ADDR_OFFSET, 0x1600);
for(UINT u=0; u<ArraySize(regs); u++)
	MdioWrite(0, regs[u], data[u]);
SleepMicroseconds(100);
Bits::Set(pcie->PL_PHY_CTL_15, PL_PHY_CTL_15_CLOCK_PERIOD_MASK, PL_PHY_CTL_15_CLOCK_PERIOD_DEFAULT);
UINT misc_ctrl=Bits::Get(pcie->MISC_CTRL);
Bits::Set(misc_ctrl, MISC_CTRL_CFG_READ_UR_MODE|MISC_CTRL_RCB_MPS_MODE|MISC_CTRL_SCB_ACCESS_EN);
Bits::Set(misc_ctrl, MISC_CTRL_MAX_BURST_SIZE, MISC_CTRL_MAX_BURST_SIZE_256);
Bits::Write(pcie->MISC_CTRL, misc_ctrl);
Bits::Clear(pcie->AXI_INTF_CTRL, AXI_INTF_CTRL_REQFIFO_EN_QOS_PROPAGATION);
Bits::Clear(pcie->MISC_CTRL1, MISC_CTRL1_EN_VDM_QOS_CONTROL);
Bits::Set(pcie->MISC_CTRL1, MISC_CTRL1_EN_VDM_QOS_CONTROL);
constexpr UINT QOS_MAP=0xBBAA9888U;
Bits::Write(pcie->VDM_PRIORITY_TO_QOS_MAP_LO, QOS_MAP);
Bits::Write(pcie->VDM_PRIORITY_TO_QOS_MAP_HI, QOS_MAP);
Bits::Clear(pcie->CFG.TL_VDM_CTL1);
Bits::Set(pcie->CFG.TL_VDM_CTL0, TL_VDM_CTL0_VDM_ENABLED|TL_VDM_CTL0_VDM_IGNORETAG|TL_VDM_CTL0_VDM_IGNOREVNDRID);
Bits::Write(pcie->RC_BAR2_CONFIG_LO, LowLong(RBAR2_OFFSET)|EncodeBarSize(RBAR2_SIZE));
Bits::Write(pcie->RC_BAR2_CONFIG_HI, HighLong(RBAR2_OFFSET));
Bits::Set(pcie->UBUS_BAR2_CONFIG_REMAP, UBUS_BAR_CONFIG_REMAP_ACCESS_EN);
Bits::Set(pcie->MISC_CTRL, MISC_CTRL_SCB0_SIZE, GetBitCount(RBAR2_SIZE)-15);
Bits::Set(pcie->UBUS_CTRL, UBUS_CTRL_REPLY_ERR_DIS|UBUS_CTRL_REPLY_DECERR_DIS);
Bits::Write(pcie->AXI_READ_ERROR_DATA, 0xFFFFFFFF);
Bits::Write(pcie->UBUS_TIMEOUT, 0xB2D0000);
Bits::Write(pcie->RC_CONFIG_RETRY_TIMEOUT, 0xABA0000);
Bits::Clear(pcie->RC_BAR1_CONFIG_LO, RC_BAR_CONFIG_LO_SIZE);
Bits::Clear(pcie->RC_BAR3_CONFIG_LO, RC_BAR_CONFIG_LO_SIZE);
Bits::Set(pcie->CFG.PRIV1_LINK_CAPABILITY, PRIV1_LINK_CAPABILITY_ASPM_SUPPORT, LINK_STATE_L0S|LINK_STATE_L1);
Bits::Set(pcie->CFG.LNKCAP, LNKCAP_SLS, PCIE_GEN);
Bits::Set(pcie->CFG.LNKCTL2, LNKCTL2_SLS, PCIE_GEN);
Bits::Set(pcie->CFG.PRIV1_ID_VAL3, PRIV1_ID_VAL3_CLASS_CODE, PCIE_HOST_CLASS);
SetOutboundWindow(0, OUTWND_OFFSET, 0, OUTWND_SIZE);
Bits::Set(pcie->CFG.VENDOR_SPECIFIC_REG1, VENDOR_SPECIFIC_REG1_ENDIAN_MODE_BAR2, VENDOR_SPECIFIC_REG1_LITTLE_ENDIAN);
Bits::Write(pcie->RC_BAR1_CONFIG_LO, LowLong(RBAR1_OFFSET)|EncodeBarSize(RBAR1_SIZE));
Bits::Write(pcie->RC_BAR1_CONFIG_HI, HighLong(RBAR1_OFFSET));
Bits::Write(pcie->UBUS_BAR1_CONFIG_REMAP, LowLong(AXI_MSI_BASE)|UBUS_BAR_CONFIG_REMAP_ACCESS_EN);
Bits::Write(pcie->UBUS_BAR1_CONFIG_REMAP_HI, HighLong(AXI_MSI_BASE));
Bits::Set(pcie->PCIE_CTRL, PCIE_CTRL_PERST);
UINT64 timeout=GetTickCount64()+1000;
while(!IsConnected())
	{
	if(GetTickCount64()>timeout)
		throw DeviceNotReadyException();
	Sleep(100);
	}
Bits::Clear(pcie->HARD_DEBUG, HARD_DEBUG_CLKREQ_DEBUG_EN|HARD_DEBUG_CLKREQ_L1SS_EN);
assert(Bits::Get(pcie->CFG.CLASSREV, CLASSREV_CLASS)==PCIE_HOST_CLASS);
assert(Bits::Get(pcie->CFG.HEADER_TYPE)==HEADER_TYPE_BRIDGE);
Bits::Write(pcie->CFG.CACHE_LINE_SIZE, CACHE_LINE_SIZE/4);
Bits::Write(pcie->CFG.SECONDARY_BUS, 1);
Bits::Write(pcie->CFG.SUBORDINATE_BUS, 1);
Bits::Write(pcie->CFG.MEM_BASE, 0);
Bits::Write(pcie->CFG.MEM_LIMIT, 0);
Bits::Write(pcie->CFG.BRIDGE_CTRL, BRIDGE_CTRL_PARITY);
Bits::Write(pcie->CFG.ROOT_CTRL, ROOT_CTRL_CRSSVE);
Bits::Write(pcie->CFG.CMD, CMD_MEMORY|CMD_MASTER|CMD_PARITY|CMD_SERR);
}

BOOL PcieHost::IsConnected()
{
auto pcie=(PCIE_HOST_REGS*)AXI_PCIE_HOST_BASE;
UINT status=Bits::Get(pcie->PCIE_STATUS);
if(!Bits::Get(status, PCIE_STATUS_DL_ACTIVE))
	return false;
if(!Bits::Get(status, PCIE_STATUS_PHYLINKUP))
	return false;
return true;
}

SIZE_T PcieHost::MapConfiguration(UINT bus, UINT slot, UINT func)
{
auto pcie=(PCIE_HOST_REGS*)AXI_PCIE_HOST_BASE;
UINT id=0;
Bits::Set(id, CFG_INDEX_BUS, bus);
Bits::Set(id, CFG_INDEX_SLOT, slot);
Bits::Set(id, CFG_INDEX_FUNC, func);
Bits::Write(pcie->CFG_INDEX, id);
SleepMicroseconds(10);
return AXI_PCIE_HOST_BASE+0x8000;
}

UINT PcieHost::MdioAddress(UINT port, UINT reg, UINT cmd)
{
UINT addr=0;
addr|=port<<MDIO_PORT_SHIFT;
addr|=cmd<<MDIO_CMD_SHIFT;
addr|=reg;
return addr;
}

VOID PcieHost::MdioRead(UINT port, BYTE reg, UINT* value_ptr)
{
auto pcie=(PCIE_HOST_REGS*)AXI_PCIE_HOST_BASE;
UINT addr=MdioAddress(port, reg, MDIO_CMD_READ);
Bits::Write(pcie->DL_MDIO_ADDR, addr);
Bits::Get(pcie->DL_MDIO_ADDR);
for(UINT retry=0; retry<10; retry++)
	{
	if(Bits::Get(pcie->DL_MDIO_RD_DATA, MDIO_DATA_DONE))
		return;
	}
throw DeviceNotReadyException();
}

VOID PcieHost::MdioWrite(UINT port, BYTE reg, WORD value)
{
auto pcie=(PCIE_HOST_REGS*)AXI_PCIE_HOST_BASE;
UINT addr=MdioAddress(port, reg, MDIO_CMD_WRITE);
Bits::Write(pcie->DL_MDIO_ADDR, addr);
Bits::Get(pcie->DL_MDIO_ADDR);
UINT write=(UINT)value|MDIO_DATA_DONE;
Bits::Write(pcie->DL_MDIO_WR_DATA, write);
for(UINT retry=0; retry<10; retry++)
	{
	if(!Bits::Get(pcie->DL_MDIO_WR_DATA, MDIO_DATA_DONE))
		return;
	}
throw DeviceNotReadyException();
}

VOID PcieHost::Reset()
{
auto rescal=(RESCAL_REGS*)AXI_RESET_RESCAL_BASE;
Bits::Set(rescal->START, RESCAL_START);
if(!Bits::Get(rescal->START, RESCAL_START))
	throw DeviceNotReadyException();
UINT64 timeout=GetTickCount64()+10;
while(!Bits::Get(rescal->STATUS, RESCAL_STATUS))
	{
	if(GetTickCount64()>timeout)
		throw DeviceNotReadyException();
	SleepMicroseconds(10);
	}
Bits::Clear(rescal->START, RESCAL_START);
using System=Devices::System::System;
System::Reset(ResetDevice::PcieHost);
}

VOID PcieHost::SetOutboundWindow(UINT id, SIZE_T cpu_addr, SIZE_T pcie_addr, SIZE_T size)
{
auto pcie=(PCIE_HOST_REGS*)AXI_PCIE_HOST_BASE;
Bits::Write(pcie->CPU2_MEM_WIN[id].LOW, LowLong(pcie_addr));
Bits::Write(pcie->CPU2_MEM_WIN[id].HIGH, HighLong(pcie_addr));
SIZE_T base_mb=cpu_addr>>20;
SIZE_T limit_mb=(cpu_addr+size-1)>>20;
Bits::Set(pcie->CPU2_MEM_WIN_BASE_LIMIT[id], CPU2_MEM_WIN_BASE_LIMIT_BASE, base_mb);
Bits::Set(pcie->CPU2_MEM_WIN_BASE_LIMIT[id], CPU2_MEM_WIN_BASE_LIMIT_LIMIT, limit_mb);
base_mb>>=CPU2_MEM_WIN_BASE_LIMIT_NUM_BITS;
limit_mb>>=CPU2_MEM_WIN_BASE_LIMIT_NUM_BITS;
Bits::Set(pcie->CPU2_MEM_WIN_BASE_LIMIT_HI[id].BASE, CPU2_MEM_WIN_BASE_LIMIT_HI_MASK, base_mb);
Bits::Set(pcie->CPU2_MEM_WIN_BASE_LIMIT_HI[id].LIMIT, CPU2_MEM_WIN_BASE_LIMIT_HI_MASK, limit_mb);
}

Handle<PcieHost> PcieHost::s_Default;

}}
