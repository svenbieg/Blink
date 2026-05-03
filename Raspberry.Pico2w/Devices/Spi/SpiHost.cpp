//=============
// SpiHost.cpp
//=============

#include "SpiHost.h"


//=======
// Using
//=======

#include "Devices/Spi/PL022.h"
#include "Devices/System/System.h"
#include "Devices/Peripherals.h"

using namespace Concurrency;
using namespace Devices::Dma;
using namespace Devices::Gpio;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Spi {


//===============
// Configuration
//===============

typedef struct
{
SIZE_T BASE;
GpioPin RX;
GpioPin CS;
GpioPin SCK;
GpioPin TX;
DmaRequest DMA_TX;
DmaRequest DMA_RX;
ResetDevice RESET;
}SPI_DEVICE;

const SPI_DEVICE SPI_DEVICES[]=
	{
	{ SPI0_BASE, GpioPin::Gpio16, GpioPin::Gpio17, GpioPin::Gpio18, GpioPin::Gpio19, DmaRequest::Spi0Tx, DmaRequest::Spi0Rx, ResetDevice::Spi0 },
	{ SPI1_BASE, GpioPin::Gpio12, GpioPin::Gpio13, GpioPin::Gpio14, GpioPin::Gpio15, DmaRequest::Spi1Tx, DmaRequest::Spi1Rx, ResetDevice::Spi1 }
	};

const UINT SPI_COUNT=TypeHelper::ArraySize(SPI_DEVICES);

typedef struct
{
BYTE PRESCAL; // 2..254 (%2)
BYTE DIVISOR; // 0..255 (+1)
}CLK_DIV;

const CLK_DIV CLK_DIVS[]=
	{
	{ 2, 0 } // SYS_CLK/2=75MHz
	};

const UINT SPI_DATA_BITS[]={ 7, 15 };
const UINT SPI_DATA_SIZE[]={ 1, 2 };
const DmaDataSize SPI_DATA_SIZE_DMA[]={ DmaDataSize::Bits8, DmaDataSize::Bits16 };


//=========
// Globals
//=========

Mutex g_Mutex;
SpiHost* g_SpiHosts[SPI_COUNT]={ nullptr };


//==================
// Con-/Destructors
//==================

Handle<SpiHost> SpiHost::Create(SpiDevice device)
{
WriteLock lock(g_Mutex);
UINT id=(UINT)device;
if(g_SpiHosts[id])
	throw AccessDeniedException();
auto spi_host=Object::Create<SpiHost>(device);
g_SpiHosts[id]=spi_host;
return spi_host;
}

SpiHost::~SpiHost()
{
WriteLock lock(g_Mutex);
System::System::Disable(SPI_DEVICES[m_Id].RESET);
g_SpiHosts[m_Id]=nullptr;
}


//========
// Common
//========

VOID SpiHost::Read(VOID* buf, SIZE_T size, UINT timeout)
{
auto spi=(PL022_REGS*)m_Device;
m_InputDma->BeginRead(m_InputRequest, &spi->DATA, buf, size);
m_InputDma->Wait(timeout);
}

VOID SpiHost::SetBaudRate(SpiBaudRate baud)
{
UINT id=(UINT)baud;
auto spi=(PL022_REGS*)SPI_DEVICES[m_Id].BASE;
IoHelper::Set(spi->PRESCAL, CTRL0_SCR, CLK_DIVS[id].PRESCAL);
IoHelper::Set(spi->CTRL0, CTRL0_SCR, CLK_DIVS[id].DIVISOR);
}

VOID SpiHost::SetDataSize(SpiDataSize data_size)
{
UINT id=(UINT)data_size;
auto spi=(PL022_REGS*)SPI_DEVICES[m_Id].BASE;
IoHelper::Set(spi->CTRL0, CTRL0_DSS, SPI_DATA_BITS[id]);
m_InputDma->SetDataSize(SPI_DATA_SIZE_DMA[id]);
m_OutputDma->SetDataSize(SPI_DATA_SIZE_DMA[id]);
m_DataSize=SPI_DATA_SIZE[id];
}

VOID SpiHost::SetEnabled(BOOL enabled)
{
auto spi=(PL022_REGS*)SPI_DEVICES[m_Id].BASE;
UINT value=enabled? CTRL1_EN: 0;
IoHelper::Set(spi->CTRL1, CTRL1_EN, value);
}

VOID SpiHost::Write(VOID const* buf, SIZE_T size, UINT timeout)
{
auto spi=(PL022_REGS*)m_Device;
m_OutputDma->BeginWrite(m_OutputRequest, &spi->DATA, buf, size);
m_OutputDma->Wait(timeout);
}


//==========================
// Con-/Destructors Private
//==========================

SpiHost::SpiHost(SpiDevice device):
m_DataSize(1),
m_Id((UINT)device)
{
m_Device=SPI_DEVICES[m_Id].BASE;
System::System::Enable(SPI_DEVICES[m_Id].RESET);
m_GpioHost=GpioHost::Create();
m_GpioHost->SetPinMode(SPI_DEVICES[m_Id].CS, GpioPinMode::Func1);
m_GpioHost->SetPinMode(SPI_DEVICES[m_Id].SCK, GpioPinMode::Func1);
m_GpioHost->SetPinMode(SPI_DEVICES[m_Id].RX, GpioPinMode::Func1);
m_GpioHost->SetPinMode(SPI_DEVICES[m_Id].TX, GpioPinMode::Func1);
m_InputDma=DmaChannel::Create();
m_InputRequest=SPI_DEVICES[m_Id].DMA_RX;
m_OutputDma=DmaChannel::Create();
m_OutputRequest=SPI_DEVICES[m_Id].DMA_TX;
}

}}