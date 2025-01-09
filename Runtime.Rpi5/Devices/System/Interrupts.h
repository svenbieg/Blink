//==============
// Interrupts.h
//==============

#pragma once


//=======
// Using
//=======

#include <config.h>
#include "Peripherals.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//=========
// Handler
//=========

typedef VOID (*IRQ_HANDLER)(VOID* Parameter);


//========
// Target
//========

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
	static VOID Disable()noexcept;
	static VOID Disable(UINT Irq)noexcept;
	static VOID Enable()noexcept;
	static VOID Enable(UINT Irq)noexcept;
	static VOID HandleInterrupt(UINT Irq)noexcept;
	static VOID Initialize()noexcept;
	static VOID Route(UINT Irq, IrqTarget Target)noexcept;
	static inline VOID Send(UINT Irq, UINT Core)noexcept { Send(Irq, (IrqTarget)(1<<Core)); }
	static VOID Send(UINT Irq, IrqTarget Target)noexcept;
	static VOID SetHandler(UINT Irq, IRQ_HANDLER Handler, VOID* Parameter=nullptr)noexcept;

private:
	// Common
	static UINT s_DisableCount[CPU_COUNT];
	static IRQ_HANDLER s_IrqHandler[IRQ_COUNT];
	static VOID* s_IrqParameter[IRQ_COUNT];
};

}}