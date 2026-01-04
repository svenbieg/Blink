//==============
// Interrupts.h
//==============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/System/Cpu.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//=========
// Handler
//=========

typedef VOID (*IRQ_HANDLER)(VOID* Parameter);


//======
// IRQs
//======

constexpr UINT IRQ_COUNT=512;

enum class Irq
{
TaskSwitch		=0,
SystemTimer		=30,
Dma0			=0x50+0x20,
Dma1			=0x51+0x20,
Dma2			=0x52+0x20,
Dma3			=0x53+0x20,
Dma4			=0x54+0x20,
Dma5			=0x55+0x20,
Dma6			=0x56+0x20,
Dma7			=0x57+0x20,
Dma8			=0x58+0x20,
Dma9			=0x59+0x20,
Dma10			=0x5A+0x20,
Dma11			=0x5B+0x20,
UArt10			=0x79+0x20,
PcieHostIntA	=0xE5+0x20,
PcieHostMsi		=0xEA+0x20,
SdCard			=0x111+0x20,
Wifi			=0x112+0x20
};


//============
// IRQ-Target
//============

enum class IrqTarget
{
All=0xF,
Secondary=0xE,
Core0=(1<<0),
Core1=(1<<1),
Core2=(1<<2),
Core3=(1<<3)
};


//============
// Interrupts
//============

class Interrupts
{
public:
	// Common
	static BOOL Active();
	static VOID Disable();
	static VOID Enable();
	static BOOL Enabled();
	static VOID HandleInterrupt(UINT Irq);
	static VOID Initialize();
	static VOID InitializeSecondary();
	static VOID Route(Irq Irq, IrqTarget Target);
	static inline VOID Send(Irq Irq, UINT Core) { Send(Irq, (IrqTarget)(1<<Core)); }
	static VOID Send(Irq Irq, IrqTarget Target);
	static VOID SetHandler(Irq Irq, IRQ_HANDLER Handler, VOID* Parameter=nullptr);

private:
	// Using
	static constexpr UINT CPU_COUNT=Devices::System::Cpu::CPU_COUNT;

	// Common
	static VOID Disable(UINT Irq);
	static VOID Enable(UINT Irq);
	static BOOL s_Active[CPU_COUNT];
	static Concurrency::CriticalSection s_CriticalSection;
	static UINT s_DisableCount[CPU_COUNT];
	static IRQ_HANDLER s_IrqHandlers[IRQ_COUNT];
	static VOID* s_IrqParameters[IRQ_COUNT];
};

}}