//==============
// Interrupts.h
//==============

#pragma once


//=======
// Using
//=======

#include <config.h>
#include <irq.h>


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//=========
// Handler
//=========

typedef VOID (*IRQ_HANDLER)(VOID* Parameter);


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
	static VOID Disable(UINT Irq);
	static VOID Enable();
	static VOID Enable(UINT Irq);
	static BOOL Enabled();
	static VOID HandleInterrupt(UINT Irq);
	static VOID Initialize();
	static VOID Route(UINT Irq, IrqTarget Target);
	static inline VOID Send(UINT Irq, UINT Core) { Send(Irq, (IrqTarget)(1<<Core)); }
	static VOID Send(UINT Irq, IrqTarget Target);
	static VOID SetHandler(UINT Irq, IRQ_HANDLER Handler, VOID* Parameter=nullptr);

private:
	// Common
	static BOOL s_Active[CPU_COUNT];
	static UINT s_DisableCount[CPU_COUNT];
	static IRQ_HANDLER s_IrqHandler[IRQ_COUNT];
	static VOID* s_IrqParameter[IRQ_COUNT];
};

}}