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
	VOID Read(VOID* Buffer, SIZE_T Size, UINT Timeout=100);
	VOID SetBaudRate(SpiBaudRate Baud);
	VOID SetDataSize(SpiDataSize DataSize);
	VOID SetEnabled(BOOL Enabled);
	VOID Write(VOID const* Buffer, SIZE_T Size, UINT Timeout=100);

private:
	// Con-/Destructors
	friend Object;
	SpiHost(SpiDevice Device);

	// Common
	UINT m_DataSize;
	SIZE_T m_Device;
	Handle<GpioHost> m_GpioHost;
	UINT m_Id;
	Handle<DmaChannel> m_InputDma;
	DmaRequest m_InputRequest;
	Handle<DmaChannel> m_OutputDma;
	DmaRequest m_OutputRequest;
};

}}