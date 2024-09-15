//================
// SerialPort.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "Devices/Gpio/Gpio.h"
#include "Devices/Pcie/SouthBridge.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/Peripherals.h"
#include "SerialPort.h"

using namespace Devices::Gpio;
using namespace Devices::Pcie;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Serial {


//==========
// Settings
//==========

constexpr UINT UART_CLOCK_ARM=44000000;
constexpr UINT UART_CLOCK_RP1=50000000;

constexpr UINT UART_COUNT=11;
constexpr UINT UART_TIMEOUT=10;


//=========
// Globals
//=========

Handle<SerialPort> g_SerialPort[UART_COUNT];


//======
// Info
//======

typedef struct
{
SIZE_T BASE;
BYTE TX_PIN;
BYTE RX_PIN;
PinMode TX_ALT;
PinMode RX_ALT;
UINT IRQ;
}UART_INFO;

constexpr UART_INFO UArtInfo[UART_COUNT]=
	{
	{ RP1_UART0_BASE, 14, 15, PinMode::Alt4, PinMode::Alt4, 0 },
	{ RP1_UART1_BASE, 0, 1, PinMode::Alt2, PinMode::Alt2, 0 },
	{ RP1_UART2_BASE, 4, 5, PinMode::Alt2, PinMode::Alt2, 0 },
	{ RP1_UART3_BASE, 8, 9, PinMode::Alt2, PinMode::Alt2, 0 },
	{ RP1_UART4_BASE, 12, 13, PinMode::Alt2, PinMode::Alt2, 0 },
	{ RP1_UART5_BASE, 36, 37, PinMode::Alt1, PinMode::Alt1, 0 },
	{ 0 },
	{ 0 },
	{ 0 },
	{ 0 },
	{ ARM_UART_BASE, 0, 0, PinMode::Input, PinMode::Input, IRQ_UART10 },
	};


//===========
// Registers
//===========

typedef struct
{
REG DATA;
REG STATUS;
REG RES0[4];
REG FLAGS;
REG RES1;
REG ILPR;
REG IBRD;
REG FBRD;
REG LCRH; // Line Control
REG CTRL;
REG IFLS;
REG IMSC;
REG RIS;
REG MIS;
REG ICR;
REG DMACR;
}PL011_REGS;

constexpr UINT CTRL_RX_ENABLE=(1<<9);
constexpr UINT CTRL_TX_ENABLE=(1<<8);
constexpr UINT CTRL_ENABLE=(1<<0);

constexpr UINT FLAG_TX_EMPTY=(1<<7);
constexpr UINT FLAG_RX_FULL=(1<<6);
constexpr UINT FLAG_TX_FULL=(1<<5);
constexpr UINT FLAG_RX_EMPTY=(1<<4);
constexpr UINT FLAG_BUSY=(1<<3);

constexpr UINT LCRH_WORD_LEN_8=3<<5;
constexpr UINT LCRH_FIFO_ENABLE=1<<4;


//===========
// Baud Rate
//===========

inline UINT BaudInt(UINT clock, UINT baud)
{
return clock/(baud<<4);
}

inline UINT BaudFrac(UINT clock, UINT baud)
{
UINT baud16=baud<<4;
UINT baud_int=clock/baud16;
UINT baud_frac=(clock%baud16)*8/baud;
return baud_frac/2+baud_frac%2;
}


//========
// Common
//========

Handle<SerialPort> SerialPort::Open(UArtDevice device, BaudRate baud)
{
UINT id=(UINT)device;
if(g_SerialPort[id])
	return g_SerialPort[id];
if(device!=UArtDevice::UArt10)
	{
	auto rp1=SouthBridge::Open();
	if(!rp1)
		return nullptr;
	}
g_SerialPort[id]=new SerialPort(device, baud);
return g_SerialPort[id];
}

BOOL SerialPort::Poll()
{
auto uart=(PL011_REGS*)m_Address;
return Bits::Get(uart->FLAGS, FLAG_RX_EMPTY)==0;
}


//==============
// Input-Stream
//==============

SIZE_T SerialPort::Available()
{
auto uart=(PL011_REGS*)m_Address;
if(Bits::Get(uart->FLAGS, FLAG_RX_EMPTY))
	return 0;
return 1;
}

SIZE_T SerialPort::Read(VOID* buf, SIZE_T size)
{
BYTE* buf_ptr=(BYTE*)buf;
for(SIZE_T u=0; u<size; u++)
	buf_ptr[u]=Read();
return size;
}


//===============
// Output-Stream
//===============

VOID SerialPort::Flush()
{
auto uart=(PL011_REGS*)m_Address;
UINT64 timeout=GetTickCount64()+UART_TIMEOUT;
while(Bits::Get(uart->FLAGS, FLAG_BUSY))
	{
	if(GetTickCount64()>timeout)
		throw TimeoutException();
	}
}

SIZE_T SerialPort::Write(VOID const* buf, SIZE_T size)
{
BYTE const* buf_ptr=(BYTE const*)buf;
for(SIZE_T u=0; u<size; u++)
	Write(buf_ptr[u]);
return size;
}


//==========================
// Con-/Destructors Private
//==========================

SerialPort::SerialPort(UArtDevice device, BaudRate baud):
m_BaudRate(baud),
m_Device(device)
{
UINT id=(UINT)device;
m_Address=UArtInfo[id].BASE;
if(UArtInfo[id].RX_PIN)
	{
	using Gpio=Devices::Gpio::Gpio;
	Gpio::SetPinMode(UArtInfo[id].TX_PIN, UArtInfo[id].TX_ALT);
	Gpio::SetPinMode(UArtInfo[id].RX_PIN, UArtInfo[id].RX_ALT, PullMode::PullUp);
	}
auto uart=(PL011_REGS*)UArtInfo[id].BASE;
Bits::Clear(uart->CTRL, CTRL_ENABLE);
UINT timeout=GetTickCount64()+UART_TIMEOUT;
while(Bits::Get(uart->FLAGS, FLAG_BUSY))
	{
	if(GetTickCount64()>timeout)
		throw DeviceNotReadyException();
	}
Bits::Clear(uart->LCRH, LCRH_FIFO_ENABLE);
UINT clock=UART_CLOCK_RP1;
if(id==10)
	clock=UART_CLOCK_ARM;
Bits::Write(uart->IMSC, 0);
Bits::Write(uart->ICR, 0x7FF);
Bits::Write(uart->IBRD, BaudInt(clock, (UINT)baud));
Bits::Write(uart->FBRD, BaudFrac(clock, (UINT)baud));
Bits::Write(uart->LCRH, LCRH_WORD_LEN_8|LCRH_FIFO_ENABLE);
Bits::Set(uart->CTRL, CTRL_RX_ENABLE|CTRL_TX_ENABLE|CTRL_ENABLE);
}


//================
// Common Private
//================

BYTE SerialPort::Read()
{
auto uart=(PL011_REGS*)m_Address;
UINT64 timeout=GetTickCount64()+UART_TIMEOUT;
while(Bits::Get(uart->FLAGS, FLAG_RX_EMPTY))
	{
	if(GetTickCount64()>timeout)
		throw TimeoutException();
	}
return (BYTE)Bits::Get(uart->DATA);
}

VOID SerialPort::Write(BYTE value)
{
auto uart=(PL011_REGS*)m_Address;
UINT64 timeout=GetTickCount64()+UART_TIMEOUT;
while(Bits::Get(uart->FLAGS, FLAG_TX_FULL))
	{
	if(GetTickCount64()>timeout)
		throw TimeoutException();
	}
Bits::Write(uart->DATA, value);
}

}}
