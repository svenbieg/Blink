//===============
// SpiEmulator.h
//===============

#pragma once


//=======
// Using
//=======

#include "Devices/Dma/DmaChannel.h"
#include "Devices/Gpio/GpioHost.h"
#include "Devices/Pio/StateMachine.h"
#include "Devices/Spi/SpiHost.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Pio {


//===============
// Configuration
//===============

enum class SpiMode: BYTE
{
Bits8,
Bits16,
Bits32
};

typedef struct
{
using GpioPin=Devices::Gpio::GpioPin;
BYTE Divisor;
SpiMode Mode;
GpioPin PinChipSelect;
GpioPin PinClock;
GpioPin PinRx;
GpioPin PinTx;
}SPI_CONFIG;


//==============
// SPI-Emulator
//==============

class SpiEmulator: public Spi::SpiHost
{
public:
	// Using
	using DmaChannel=Devices::Dma::DmaChannel;
	using DmaRequest=Devices::Dma::DmaRequest;
	using GpioHost=Devices::Gpio::GpioHost;
	using GpioPin=Devices::Gpio::GpioPin;
	using GpioPinMode=Devices::Gpio::GpioPinMode;
	using StateMachine=Devices::Pio::StateMachine;

	// Friends
	friend Object;

	// Con-/Destructors
	static inline Handle<SpiEmulator> Create(SPI_CONFIG const& Configuration)
		{
		return new SpiEmulator(Configuration);
		}

	// Common
	VOID SpiBegin(SIZE_T TxSize, SIZE_T RxSize)override;
	VOID SpiEnd()override;
	VOID SpiRead(VOID* Buffer, SIZE_T Size)override;
	VOID SpiWrite(VOID const* Buffer, SIZE_T Count)override;

protected:
	// Con-/Destructors
	SpiEmulator(SPI_CONFIG const& Configuration);

	// Members
	UINT m_DataSize;
	Handle<DmaChannel> m_DmaChannel;
	Handle<GpioHost> m_GpioHost;
	RO32* m_InputBuffer;
	DmaRequest m_InputRequest;
	GpioPin m_PinChipSelect;
	RW32* m_OutputBuffer;
	DmaRequest m_OutputRequest;
	GpioPinMode m_PinMode;
	Handle<StateMachine> m_StateMachine;
};

}}