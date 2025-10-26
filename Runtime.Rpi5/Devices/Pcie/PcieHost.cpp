//==============
// PcieHost.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Devices/Pcie/PcieHost.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/System.h"

using namespace Concurrency;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Pcie {


//========
// Common
//========

VOID PcieHost::SetInterruptHandler(Rp1Irq rp1_irq, IRQ_HANDLER handler, VOID* param)
{
UINT irq=(UINT)rp1_irq;
UINT64 irq_mask=1ULL<<irq;
auto intc=(rp1_intc_regs_t*)RP1_INTC_BASE;
SpinLock lock(m_CriticalSection);
if(handler)
	{
	m_IrqHandlers[irq]=handler;
	m_IrqParameters[irq]=param;
	if(FlagHelper::Get(irq_mask, RP1_IRQ_EDGE_MASK))
		{
		io_write(intc->MSIX_CFG_CLEAR[irq], MSIX_CFG_IACK_EN);
		}
	else
		{
		io_write(intc->MSIX_CFG_SET[irq], MSIX_CFG_IACK_EN);
		}
	io_write(intc->MSIX_CFG_SET[irq], MSIX_CFG_EN);
	}
else
	{
	io_write(intc->MSIX_CFG_CLEAR[irq], MSIX_CFG_EN);
	io_write(intc->MSIX_CFG_CLEAR[irq], MSIX_CFG_IACK_EN);
	m_IrqHandlers[irq]=nullptr;
	m_IrqParameters[irq]=nullptr;
	}
}


//==========================
// Con-/Destructors Private
//==========================

PcieHost::PcieHost()
{
MemoryHelper::Fill(m_IrqHandlers, RP1_IRQ_COUNT*sizeof(IRQ_HANDLER), 0);
MemoryHelper::Fill(m_IrqParameters, RP1_IRQ_COUNT*sizeof(VOID*), 0);
Initialize();
}

Global<PcieHost> PcieHost::s_Current;


//================
// Common Private
//================

VOID PcieHost::EnableDevice(UINT id, BYTE slot, BYTE func)
{
auto pcie=(pcie_regs_t*)MapConfiguration(1, slot, func);
if(io_read(pcie->CLASSREV, CLASSREV_CLASS)!=id)
	throw DeviceNotReadyException();
if(io_read(pcie->HEADER_TYPE)!=HEADER_TYPE_NORMAL)
	throw DeviceNotReadyException();
io_write(pcie->CACHE_LINE_SIZE, Cpu::CACHE_LINE_SIZE/4);
io_write(pcie->BASE_LO, TypeHelper::LowLong(PCIE_OFFSET)|BASE_MEM_TYPE_64);
io_write(pcie->BASE_HI, TypeHelper::HighLong(PCIE_OFFSET));
io_write(pcie->IRQ_PIN, 1);
io_write(pcie->CMD, CMD_MEMORY|CMD_MASTER|CMD_PARITY|CMD_SERR);
}

UINT PcieHost::EncodeBarSize(UINT64 size)
{
UINT bits=bits_count(size);
if(bits<12)
	return 0;
if(bits<16)
	return (bits-12)+0x1C; // 4kB..32kB
if(bits<38)
	return bits-15; // 64kB..32GB
return 0;
}

VOID PcieHost::HandleInterrupt(VOID* param)
{
auto pcie_host=(PcieHost*)param;
pcie_host->OnInterrupt();
}

VOID PcieHost::Initialize()
{
auto pcie=(pcie_host_regs_t*)AXI_PCIE_HOST_BASE;
constexpr BYTE regs[]={ 0x16, 0x17, 0x18, 0x19, 0x1B, 0x1C, 0x1E };
constexpr WORD data[]={ 0x50B9, 0xBDA1, 0x0094, 0x97B4, 0x5030, 0x5030, 0x0007 };
MdioWrite(0, MDIO_REG_SET_ADDR_OFFSET, 0x1600);
for(UINT u=0; u<TypeHelper::ArraySize(regs); u++)
	MdioWrite(0, regs[u], data[u]);
Task::Sleep(10);
io_write(pcie->PL_PHY_CTL_15, PL_PHY_CTL_15_CLOCK_PERIOD, PL_PHY_CTL_15_CLOCK_PERIOD_DEFAULT);
UINT misc_ctrl=io_read(pcie->MISC_CTRL);
bits_set(misc_ctrl, MISC_CTRL_CFG_READ_UR_MODE|MISC_CTRL_RCB_MPS_MODE|MISC_CTRL_SCB_ACCESS_EN);
bits_set(misc_ctrl, MISC_CTRL_MAX_BURST_SIZE, MISC_CTRL_MAX_BURST_SIZE_256);
io_write(pcie->MISC_CTRL, misc_ctrl);
io_clear(pcie->AXI_INTF_CTRL, AXI_INTF_CTRL_REQFIFO_EN_QOS_PROPAGATION);
io_clear(pcie->MISC_CTRL1, MISC_CTRL1_EN_VDM_QOS_CONTROL);
io_set(pcie->MISC_CTRL1, MISC_CTRL1_EN_VDM_QOS_CONTROL);
constexpr UINT QOS_MAP=0xBBAA9888U;
io_write(pcie->VDM_PRIORITY_TO_QOS_MAP_LO, QOS_MAP);
io_write(pcie->VDM_PRIORITY_TO_QOS_MAP_HI, QOS_MAP);
io_write(pcie->CFG.TL_VDM_CTL1, 0);
io_set(pcie->CFG.TL_VDM_CTL0, TL_VDM_CTL0_VDM_ENABLED|TL_VDM_CTL0_VDM_IGNORETAG|TL_VDM_CTL0_VDM_IGNOREVNDRID);
io_write(pcie->RC_BAR2_CONFIG_LO, TypeHelper::LowLong(RBAR2_OFFSET)|EncodeBarSize(RBAR2_SIZE));
io_write(pcie->RC_BAR2_CONFIG_HI, TypeHelper::HighLong(RBAR2_OFFSET));
io_write(pcie->MISC_CTRL, MISC_CTRL_SCB0_SIZE, bits_count(RBAR2_SIZE)-15);
io_set(pcie->UBUS_CTRL, UBUS_CTRL_REPLY_ERR_DIS|UBUS_CTRL_REPLY_DECERR_DIS);
io_write(pcie->AXI_READ_ERROR_DATA, 0xFFFFFFFF);
io_write(pcie->UBUS_TIMEOUT, 0xB2D0000);
io_write(pcie->RC_CONFIG_RETRY_TIMEOUT, 0xABA0000);
io_write(pcie->CFG.PRIV1_LINK_CAPABILITY, PRIV1_LINK_CAPABILITY_ASPM_SUPPORT, LINK_STATE_L0S|LINK_STATE_L1);
SetOutboundWindow(0, OUTWND_OFFSET, PCIE_OFFSET, OUTWND_SIZE);
io_write(pcie->RC_BAR1_CONFIG_LO, TypeHelper::LowLong(RBAR1_OFFSET)|EncodeBarSize(RBAR1_SIZE));
io_write(pcie->RC_BAR1_CONFIG_HI, TypeHelper::HighLong(RBAR1_OFFSET));
io_write(pcie->UBUS_BAR1_CONFIG_REMAP, TypeHelper::LowLong(AXI_MSI_BASE)|UBUS_BAR_CONFIG_REMAP_ACCESS_EN);
io_write(pcie->UBUS_BAR1_CONFIG_REMAP_HI, TypeHelper::HighLong(AXI_MSI_BASE));
io_set(pcie->PCIE_CTRL, PCIE_CTRL_PERST);
Task::Sleep(100);
if(!IsConnected())
	throw DeviceNotReadyException();
io_clear(pcie->HARD_DEBUG, HARD_DEBUG_CLKREQ_L1SS_EN);
auto bridge=(pcie_regs_t*)pcie;
assert(io_read(bridge->CLASSREV, CLASSREV_CLASS)==CLASSREV_CLASS_HOST);
assert(io_read(bridge->HEADER_TYPE)==HEADER_TYPE_BRIDGE);
io_write(bridge->CACHE_LINE_SIZE, Cpu::CACHE_LINE_SIZE/4);
io_write(bridge->SECONDARY_BUS, 1);
io_write(bridge->SUBORDINATE_BUS, 1);
io_write(bridge->MEM_BASE, 0);
io_write(bridge->MEM_LIMIT, 0);
io_write(bridge->BRIDGE_CTRL, BRIDGE_CTRL_PARITY);
io_write(bridge->ROOT_CTRL, ROOT_CTRL_CRSSVE);
io_write(bridge->CMD, CMD_MEMORY|CMD_MASTER|CMD_PARITY|CMD_SERR);
constexpr UINT RP1_CLASS=0x20000;
EnableDevice(RP1_CLASS, 0, 0);
Interrupts::SetHandler(Irq::PcieHostIntA, HandleInterrupt, this);
}

BOOL PcieHost::IsConnected()
{
auto pcie=(pcie_host_regs_t*)AXI_PCIE_HOST_BASE;
UINT status=io_read(pcie->PCIE_STATUS);
if(!bits_get(status, PCIE_STATUS_DL_ACTIVE))
	return false;
if(!bits_get(status, PCIE_STATUS_PHYLINKUP))
	return false;
return true;
}

SIZE_T PcieHost::MapConfiguration(UINT bus, UINT slot, UINT func)
{
auto pcie=(pcie_host_regs_t*)AXI_PCIE_HOST_BASE;
UINT id=0;
bits_set(id, CFG_INDEX_BUS, bus);
bits_set(id, CFG_INDEX_SLOT, slot);
bits_set(id, CFG_INDEX_FUNC, func);
io_write(pcie->CFG_INDEX, id);
Cpu::Delay(10);
return (SIZE_T)&pcie->CFG_DATA;
}

VOID PcieHost::MdioWrite(UINT port, BYTE reg, WORD value)
{
auto pcie=(pcie_host_regs_t*)AXI_PCIE_HOST_BASE;
UINT addr=0;
bits_set(addr, MDIO_CMD, MDIO_CMD_WRITE);
bits_set(addr, MDIO_PORT, port);
bits_set(addr, MDIO_REG, reg);
io_write(pcie->DL_MDIO_ADDR, addr);
io_read(pcie->DL_MDIO_ADDR);
UINT write=(UINT)value|MDIO_DATA_DONE;
io_write(pcie->DL_MDIO_WR_DATA, write);
for(UINT retry=0; retry<100; retry++)
	{
	if(!io_read(pcie->DL_MDIO_WR_DATA, MDIO_DATA_DONE))
		return;
	}
throw DeviceNotReadyException();
}

VOID PcieHost::OnInterrupt()
{
SpinLock lock(m_CriticalSection);
auto intc=(rp1_intc_regs_t*)RP1_INTC_BASE;
UINT64 mask=io_read(intc->INT_STAT);
for(UINT irq=0; mask!=0; irq++)
	{
	if(mask&1)
		{
		UINT64 irq_mask=1ULL<<irq;
		m_IrqHandlers[irq](m_IrqParameters[irq]);
		if(!FlagHelper::Get(irq_mask, RP1_IRQ_EDGE_MASK))
			io_write(intc->MSIX_CFG_SET[irq], MSIX_CFG_IACK);
		}
	mask>>=1;
	}
}

VOID PcieHost::SetOutboundWindow(UINT id, SIZE_T cpu_addr, SIZE_T pcie_addr, SIZE_T size)
{
auto pcie=(pcie_host_regs_t*)AXI_PCIE_HOST_BASE;
io_write(pcie->CPU2_MEM_WIN[id].LOW, TypeHelper::LowLong(pcie_addr));
io_write(pcie->CPU2_MEM_WIN[id].HIGH, TypeHelper::HighLong(pcie_addr));
SIZE_T base_mb=cpu_addr>>20;
SIZE_T limit_mb=(cpu_addr+size-1)>>20;
io_write(pcie->CPU2_MEM_WIN_BASE_LIMIT[id], CPU2_MEM_WIN_BASE_LIMIT_BASE, base_mb);
io_write(pcie->CPU2_MEM_WIN_BASE_LIMIT[id], CPU2_MEM_WIN_BASE_LIMIT_LIMIT, limit_mb);
base_mb>>=CPU2_MEM_WIN_BASE_LIMIT_NUM_BITS;
limit_mb>>=CPU2_MEM_WIN_BASE_LIMIT_NUM_BITS;
io_write(pcie->CPU2_MEM_WIN_BASE_LIMIT_HI[id].BASE, CPU2_MEM_WIN_BASE_LIMIT_HI_MASK, base_mb);
io_write(pcie->CPU2_MEM_WIN_BASE_LIMIT_HI[id].LIMIT, CPU2_MEM_WIN_BASE_LIMIT_HI_MASK, limit_mb);
}

}}