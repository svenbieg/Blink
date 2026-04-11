//============
// PcieHost.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/CriticalSection.h"
#include "Devices/Pcie/Pcie.h"
#include "Devices/System/InterruptHandler.h"
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
	// Using
	using CriticalSection=Concurrency::CriticalSection;
	using InterruptHandler=Devices::System::InterruptHandler;
	template <class _owner_t> using InterruptMemberProcedure=Devices::System::InterruptMemberProcedure<_owner_t>;

	// Con-/Destructors
	static inline Handle<PcieHost> Create() { return Global::Create(); }

	// Common
	static inline SIZE_T GetDmaAddress(VOID* Buffer) { return (SIZE_T)Buffer|DMA_OFFSET; }
	static inline SIZE_T GetDmaAddress(SIZE_T Address) { return Address|DMA_OFFSET; }
	VOID SetInterruptHandler(Rp1Irq Irq, VOID (*Procedure)());
	template <class _owner_t> inline VOID SetInterruptHandler(Rp1Irq Irq, _owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		auto handler=new InterruptMemberProcedure(Owner, Procedure);
		SetInterruptHandlerInternal(Irq, handler);
		}

private:
	// Con-/Destructors
	friend Object;
	PcieHost();

	// Common
	VOID EnableDevice(UINT Id, BYTE Slot, BYTE Function);
	UINT EncodeBarSize(UINT64 Size);
	VOID HandleInterrupt();
	VOID Initialize();
	BOOL IsConnected();
	SIZE_T MapConfiguration(UINT Bus, UINT Slot, UINT Function);
	VOID MdioWrite(UINT Port, BYTE Register, WORD Value);
	VOID SetInterruptHandlerInternal(Rp1Irq Irq, InterruptHandler* Handler)noexcept;
	VOID SetOutboundWindow(UINT Id, SIZE_T CpuAddress, SIZE_T PcieAddress, SIZE_T Size);
	CriticalSection m_CriticalSection;
	InterruptHandler* m_IrqHandlers[RP1_IRQ_COUNT];
	Handle<PcieHost> m_This;
};

}}