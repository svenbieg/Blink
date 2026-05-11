//===========
// SpiHost.h
//===========

#pragma once


//=======
// Using
//=======

#include "Devices/Dma/DmaChannel.h"
#include "Devices/Gpio/GpioHost.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Spi {


//===============
// Configuration
//===============

enum class SpiDevice
{
Spi0,
Spi1
};

enum class SpiBaudRate: UINT
{
Baud1M,
Baud15M,
Baud75M
};

enum class SpiDataSize
{
Bits8,
Bits16
};


//==========
// SPI-Host
//==========

class SpiHost: public Object
{
public:
	// Using
	using DmaChannel=Devices::Dma::DmaChannel;
	using DmaRequest=Devices::Dma::DmaRequest;
	using GpioHost=Devices::Gpio::GpioHost;

	// Con-/Destructors
	static Handle<SpiHost> Create(SpiDevice Device=SpiDevice::Spi0);
	~SpiHost();

	// Common
	VOID Read(VOID* Buffer, SIZE_T Size, UINT Timeout);
	VOID Transmit(VOID const* Tx, VOID* Rx, UINT Size);
	VOID Write(VOID const* Buffer, SIZE_T Size);
	VOID Write(VOID const* Buffer, SIZE_T Size, UINT Timeout);

protected:
	// Common
	UINT Release()noexcept override;

private:
	// Con-/Destructors
	friend Object;
	SpiHost(SpiDevice Device);

	// Common
	VOID Initialize();
	SpiBaudRate m_BaudRate;
	SpiDataSize m_DataSize;
	SIZE_T m_Device;
	Handle<GpioHost> m_GpioHost;
	UINT m_Id;
	Handle<DmaChannel> m_InputDma;
	DmaRequest m_InputRequest;
	Handle<DmaChannel> m_OutputDma;
	DmaRequest m_OutputRequest;
};

}}