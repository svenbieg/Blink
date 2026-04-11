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
#include "Devices/IoHelper.h"
#include "Handle.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Dma {


//==============
// Data-Request
//==============

enum class DmaRequest
{
Pio0Tx0, Pio0Tx1, Pio0Tx2, Pio0Tx3, Pio0Rx0, Pio0Rx1, Pio0Rx2, Pio0Rx3,
Pio1Tx0, Pio1Tx1, Pio1Tx2, Pio1Tx3, Pio1Rx0, Pio1Rx1, Pio1Rx2, Pio1Rx3,
Pio2Tx0, Pio2Tx1, Pio2Tx2, Pio2Tx3, Pio2Rx0, Pio2Rx1, Pio2Rx2, Pio2Rx3,
Spi0Tx, Spi0Rx,
Spi1Tx, Spi1Rx,
UArt0Tx, UArt0Rx,
UArt1Tx, UArt1Rx,
Pwm0, Pwm1, Pwm2, Pwm3, Pwm4, Pwm5, Pwm6,
Pwm7, Pwm8, Pwm9, Pwm10, Pwm11,
I2c0Tx,I2c0Rx,
I2c1Tx,I2c1Rx,
Adc,
XipStream, XipQmiTx, XipQmiRx,
Hstx,
CoreSight,
Sha256,
Timer0=59, Timer1, Timer2, Timer3,
Force
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
	static Handle<DmaChannel> Create();
	~DmaChannel();

	// Common
	VOID Abort();
	VOID BeginRead(DmaRequest Request, RO32* Register, UINT* Buffer, SIZE_T Count);
	VOID SetByteSwap(BOOL ByteSwap);
	VOID BeginWrite(DmaRequest Request, RW32* Register, UINT const* Buffer, SIZE_T Count);
	VOID Wait(UINT Timeout=100);

private:
	// Con-/Destructors
	DmaChannel(UINT Id);
	static Concurrency::Mutex s_Mutex;
	static UINT s_Used;

	// Common
	static UINT GetChannel();
	VOID OnInterrupt();
	Concurrency::CriticalSection m_CriticalSection;
	UINT m_Control;
	UINT m_Id;
	Concurrency::Signal m_Signal;
	Status m_Status;
};

}}