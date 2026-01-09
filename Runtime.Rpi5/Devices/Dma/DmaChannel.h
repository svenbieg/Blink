//==============
// DmaChannel.h
//==============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Mutex.h"
#include "Concurrency/Signal.h"
#include "Concurrency/SpinLock.h"
#include "Handle.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Dma {


//=========
// Devices
//=========

enum class DmaDevice
{
None=0,
Pwm1=1,
PcmTx=2,
PcmRx=3,
Smi=4,
Pwm0=5,
SpiTx=6,
SpiRx=7,
Hdmi=10,
SdCard=11,
HdmiD0=12
};


//=============
// DMA-Channel
//=============

class DmaChannel: public Object
{
public:
	// Friend
	friend Object;

	// Using
	using SpinLock=Concurrency::SpinLock;

	// Con-/Destructors
	static Handle<DmaChannel> Create(DmaDevice Device);
	~DmaChannel();

	// Common
	VOID Read(volatile UINT* Register, VOID* Buffer, SIZE_T Size, UINT Timeout=100);
	VOID Write(volatile UINT* Register, VOID const* Buffer, SIZE_T Size, UINT Timeout=100);

private:
	// Con-/Destructors
	DmaChannel(VOID* Buffer, SIZE_T Size, DmaDevice Device, UINT Id);
	static Concurrency::Mutex s_Mutex;
	static UINT s_Used;

	// Common
	static UINT GetChannel();
	static VOID HandleInterrupt(VOID* Parameter);
	VOID OnInterrupt();
	VOID* m_ControlBlock;
	Concurrency::CriticalSection m_CriticalSection;
	DmaDevice m_Device;
	UINT m_Id;
	Concurrency::Signal m_Signal;
};

}}