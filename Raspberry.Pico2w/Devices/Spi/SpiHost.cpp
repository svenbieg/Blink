//=============
// SpiHost.cpp
//=============

#include "SpiHost.h"


//=======
// Using
//=======

#include "Concurrency/ServiceTask.h"
#include "Devices/Spi/PL022.h"
#include "Devices/System/Interrupts.h"
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


//==========
// Settings
//==========

const UINT SPI_RX_BYTES=PL022_RX_BYTES*4;
const UINT SPI_TX_BYTES=PL022_TX_BYTES*4;


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
Irq IRQ;
DmaRequest DMA_TX;
DmaRequest DMA_RX;
ResetDevice RESET;
}SPI_DEVICE;

const SPI_DEVICE SPI_DEVICES[]=
	{
	{ SPI0_BASE, GpioPin::Gpio16, GpioPin::Gpio17, GpioPin::Gpio18, GpioPin::Gpio19, Irq::Spi0, DmaRequest::Spi0Tx, DmaRequest::Spi0Rx, ResetDevice::Spi0 },
	{ SPI1_BASE, GpioPin::Gpio12, GpioPin::Gpio13, GpioPin::Gpio14, GpioPin::Gpio15, Irq::Spi1, DmaRequest::Spi1Tx, DmaRequest::Spi1Rx, ResetDevice::Spi1 }
	};

const UINT SPI_COUNT=TypeHelper::ArraySize(SPI_DEVICES);

typedef struct
{
BYTE PRESCAL; // 2..254 (%2)
BYTE DIVISOR; // 0..255 (+1)
}CLK_DIV;

const CLK_DIV CLK_DIVS[]=
	{
	{ 150, 0 },	// PERI_CLK_HZ/150=1MHz
	{ 10, 0 },	// PERI_CLK_HZ/10=15MHz
	{ 2, 0 }	// PERI_CLK_HZ/2=75MHz
	};

const UINT SPI_DATA_BITS[]={ 7, 15 };
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
System::System::Disable(SPI_DEVICES[m_Id].RESET);
}


//========
// Common
//========

VOID SpiHost::Read(VOID* buf, SIZE_T size, UINT timeout)
{
auto spi=(PL022_REGS*)SPI_DEVICES[m_Id].BASE;
if(FlagHelper::Get(spi->STATUS, STATUS_BUSY))
	throw DeviceNotReadyException();
m_InputDma->BeginRead(m_InputRequest, &spi->DATA, buf, size);
m_InputDma->Wait(timeout);
}

VOID SpiHost::Transmit(VOID const* tx_buf, VOID* rx_buf, UINT size)
{
auto tx=(BYTE const*)tx_buf;
auto rx=(BYTE*)rx_buf;
auto spi=(PL022_REGS*)SPI_DEVICES[m_Id].BASE;
IoHelper::Set(spi->CTRL1, CTRL1_EN);
for(UINT u=0; u<size; u++)
	{
	IoHelper::Write(spi->DATA, tx[u]);
	rx[u]=IoHelper::Read(spi->DATA);
	}
IoHelper::Clear(spi->CTRL1, CTRL1_EN);
}

VOID SpiHost::Write(VOID const* buf, SIZE_T size)
{
auto src=(BYTE const*)buf;
auto spi=(PL022_REGS*)SPI_DEVICES[m_Id].BASE;
IoHelper::Set(spi->CTRL1, CTRL1_EN);
for(UINT u=0; u<size; u++)
	IoHelper::Write(spi->DATA, src[u]);
IoHelper::Clear(spi->CTRL1, CTRL1_EN);
}

VOID SpiHost::Write(VOID const* buf, SIZE_T size, UINT timeout)
{
auto spi=(PL022_REGS*)SPI_DEVICES[m_Id].BASE;
if(FlagHelper::Get(spi->STATUS, STATUS_BUSY))
	throw DeviceNotReadyException();
m_OutputDma->BeginWrite(m_OutputRequest, &spi->DATA, buf, size);
m_OutputDma->Wait(timeout);
}


//==================
// Common Protected
//==================

UINT SpiHost::Release()noexcept
{
WriteLock lock(g_Mutex);
UINT ref_count=Cpu::InterlockedDecrement(&m_ReferenceCount);
if(ref_count==0)
	{
	g_SpiHosts[m_Id]=nullptr;
	delete this;
	}
return ref_count;
}


//==========================
// Con-/Destructors Private
//==========================

SpiHost::SpiHost(SpiDevice device):
m_BaudRate(SpiBaudRate::Baud1M),
m_DataSize(SpiDataSize::Bits8),
m_Device(SPI_DEVICES[(UINT)device].BASE),
m_Id((UINT)device)
{
System::System::Enable(SPI_DEVICES[m_Id].RESET);
m_GpioHost=GpioHost::Create();
m_GpioHost->SetPinMode(SPI_DEVICES[m_Id].CS, GpioPinMode::Func1);
m_GpioHost->SetPinMode(SPI_DEVICES[m_Id].SCK, GpioPinMode::Func1);
m_GpioHost->SetPinMode(SPI_DEVICES[m_Id].RX, GpioPinMode::Func1);
m_GpioHost->SetPinMode(SPI_DEVICES[m_Id].TX, GpioPinMode::Func1);
auto spi=(PL022_REGS*)m_Device;
UINT ctrl0=IoHelper::Read(spi->CTRL0);
BitHelper::Set(ctrl0, CTRL0_FRF, FRF_MOTOROLA);
BitHelper::Set(ctrl0, CTRL0_SPH);
UINT baud=(UINT)m_BaudRate;
BitHelper::Set(ctrl0, CTRL0_SCR, CLK_DIVS[baud].DIVISOR);
IoHelper::Set(spi->PRESCAL, PRESCAL_DIV, CLK_DIVS[baud].PRESCAL);
UINT data_size=(UINT)m_DataSize;
BitHelper::Set(ctrl0, CTRL0_DSS, SPI_DATA_BITS[data_size]);
IoHelper::Write(spi->CTRL0, ctrl0);
m_InputDma=DmaChannel::Create();
m_InputDma->SetDataSize(SPI_DATA_SIZE_DMA[data_size]);
m_InputRequest=SPI_DEVICES[m_Id].DMA_RX;
m_OutputDma=DmaChannel::Create();
m_OutputDma->SetDataSize(SPI_DATA_SIZE_DMA[data_size]);
m_OutputRequest=SPI_DEVICES[m_Id].DMA_TX;
IoHelper::Set(spi->DMACTRL, DMACTRL_RXEN|DMACTRL_TXEN);
}

}}