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

typedef struct
{
using GpioPin=Devices::Gpio::GpioPin;
UINT Divisor;
GpioPin PinChipSelect;
GpioPin PinClock;
GpioPin PinRx;
GpioPin PinTx;
}SpiConfiguration;


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
	using StateMachine=Devices::Pio::StateMachine;

	// Con-/Destructors
	static inline Handle<SpiEmulator> Create(SpiConfiguration const& Configuration)
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
	friend Object;
	SpiEmulator(SpiConfiguration const& Configuration);

	// Members
	Handle<GpioHost> m_GpioHost;
	RO32* m_InputBuffer;
	Handle<DmaChannel> m_InputDma;
	DmaRequest m_InputRequest;
	GpioPin m_PinChipSelect;
	RW32* m_OutputBuffer;
	Handle<DmaChannel> m_OutputDma;
	DmaRequest m_OutputRequest;
	Handle<StateMachine> m_StateMachine;
};

}}