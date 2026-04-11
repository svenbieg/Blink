//===========
// WifiSpi.h
//===========

#pragma once


//=======
// Using
//=======

#include "Devices/Dma/DmaChannel.h"
#include "Devices/Gpio/GpioHost.h"
#include "Devices/Pio/StateMachine.h"
#include "Devices/Wifi/Wifi.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//=====
// SPI
//=====

const FN0_32 SPI_CTRL					=0x00;
const FN0_16 SPI_INTS					=0x04;
const FN0_16 SPI_INTE					=0x06;
const FN0_32 SPI_STATUS					=0x08;
const FN0_32 SPI_TEST					=0x14;
const FN0_08 SPI_RESP_DELAY_F1			=0x1D;

const UINT SPI_CTRL_STATUS_IRQ			=(1<<17);
const UINT SPI_CTRL_RESP_DELAY			=(1<<10);
const UINT SPI_CTRL_WAKEUP				=(1<<7);
const UINT SPI_CTRL_IRQL_HIGH			=(1<<5);
const UINT SPI_CTRL_HIGH_SPEED			=(1<<4);
const UINT SPI_CTRL_BIG_ENDIAN			=(1<<1);
const UINT SPI_CTRL_32BIT				=(1<<0);

const UINT SPI_CTRL_DEFAULT=
	SPI_CTRL_STATUS_IRQ|
	SPI_CTRL_RESP_DELAY|
	SPI_CTRL_WAKEUP|
	SPI_CTRL_IRQL_HIGH|
	SPI_CTRL_HIGH_SPEED|
	SPI_CTRL_BIG_ENDIAN|
	SPI_CTRL_32BIT;

const BITS SPI_STATUS_F2_PKT_LEN		={ 0x7FF, 9 };
const UINT SPI_STATUS_F2_PKT_AVAIL		=(1<<8);
const UINT SPI_STATUS_F2_RX_READY		=(1<<5);
const UINT SPI_STATUS_F2_IRQ			=(1<<3);
const UINT SPI_STATUS_OVERFLOW			=(1<<2);
const UINT SPI_STATUS_UNDERFLOW			=(1<<1);
const UINT SPI_STATUS_DATA_UNAVAIL		=(1<<0);

const WORD INT_F3						=(1<<11);
const WORD INT_F2						=(1<<10);
const WORD INT_F1						=(1<<9);
const WORD INT_GSPI_PKT_AVAIL			=(1<<8);
const WORD INT_F1_OVERFLOW				=(1<<7);
const WORD INT_F3_PKT_AVAIL				=(1<<6);
const WORD INT_F2_PKT_AVAIL				=(1<<5);
const WORD INT_DATA_ERR					=(1<<4);
const WORD INT_CMD_ERR					=(1<<3);
const WORD INT_F2_F3_OVERFLOW			=(1<<2);
const WORD INT_F2_F3_UNDERFLOW			=(1<<1);
const WORD INT_DATA_UNAVAIL				=(1<<0);

const WORD INT_DEFAULT=
	INT_F1_OVERFLOW|
	INT_F2_PKT_AVAIL|
	INT_DATA_ERR|
	INT_CMD_ERR|
	INT_F2_F3_OVERFLOW|
	INT_F2_F3_UNDERFLOW;


//=========
// Command
//=========

const UINT CMD_WRITE		=(1<<31);
const UINT CMD_INCR			=(1<<30);
const BITS CMD_FN			={ 0x03, 28 };
const BITS CMD_ADDR			={ 0x1FFFF, 11 };
const BITS CMD_SIZE			={ 0x3FF, 0 };

inline UINT Swap16x2(UINT Value)
{
__asm inline volatile("rev16 %0, %0": "+l" (Value));
return Value;
}


//==========
// SPI-Host
//==========

class WifiSpi: public Object
{
public:
	// Using
	using DmaChannel=Devices::Dma::DmaChannel;
	using DmaRequest=Devices::Dma::DmaRequest;
	using GpioHost=Devices::Gpio::GpioHost;
	using StateMachine=Devices::Pio::StateMachine;

protected:
	// Con-/Destructors
	friend Object;
	WifiSpi();

	// Common
	VOID SpiBegin(UINT TxCount, UINT RxCount);
	VOID SpiEnd();
	VOID SpiRead(UINT* Buffer, UINT Count);
	VOID SpiWrite(UINT const* Buffer, UINT Count);

	// Members
	Handle<GpioHost> m_GpioHost;
	RO32* m_InputBuffer;
	Handle<DmaChannel> m_InputDma;
	DmaRequest m_InputRequest;
	RW32* m_OutputBuffer;
	Handle<DmaChannel> m_OutputDma;
	DmaRequest m_OutputRequest;
	Handle<StateMachine> m_StateMachine;
};

}}