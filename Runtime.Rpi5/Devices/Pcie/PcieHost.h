//============
// PcieHost.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Pcie/Pcie.h"
#include "Global.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Pcie {


//===========
// Pcie-Host
//===========

class PcieHost: public Global<PcieHost>
{
public:
	// Con-/Destructors
	static inline Handle<PcieHost> Create() { return Global::Create(); }

	// Common
	static inline SIZE_T GetDmaAddress(VOID* Buffer) { return (SIZE_T)Buffer|DMA_OFFSET; }
	static inline SIZE_T GetDmaAddress(SIZE_T Address) { return Address|DMA_OFFSET; }
	VOID SetInterruptHandler(Rp1Irq Irq, IRQ_HANDLER Handler, VOID* Parameter=nullptr);

private:
	// Con-/Destructors
	friend Object;
	PcieHost();

	// Common
	VOID EnableDevice(UINT Id, BYTE Slot, BYTE Function);
	UINT EncodeBarSize(UINT64 Size);
	static VOID HandleInterrupt(VOID* Parameter);
	VOID Initialize();
	BOOL IsConnected();
	SIZE_T MapConfiguration(UINT Bus, UINT Slot, UINT Function);
	VOID MdioWrite(UINT Port, BYTE Register, WORD Value);
	VOID OnInterrupt();
	VOID SetOutboundWindow(UINT Id, SIZE_T CpuAddress, SIZE_T PcieAddress, SIZE_T Size);
	Concurrency::CriticalSection m_CriticalSection;
	IRQ_HANDLER m_IrqHandlers[RP1_IRQ_COUNT];
	VOID* m_IrqParameters[RP1_IRQ_COUNT];
};

}}