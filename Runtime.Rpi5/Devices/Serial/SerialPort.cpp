//================
// SerialPort.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <base.h>
#include <io.h>
#include <new>
#include "Concurrency/ServiceTask.h"
#include "Devices/Serial/SerialPort.h"

using namespace Concurrency;
using namespace Devices::Gpio;
using namespace Devices::Pcie;
using namespace Storage;


//===========
// Namespace
//===========

namespace Devices {
	namespace Serial {


//==========
// Settings
//==========

constexpr UINT UART_CLOCK=50000000;

constexpr UINT UART_INPUT_BUF=48;
constexpr UINT UART_OUTPUT_BUF=32;

constexpr UINT UART_INPUT_RING=UART_INPUT_BUF*4;


//======
// Info
//======

typedef struct
{
SIZE_T BASE;
GpioRp1Pin TX_PIN;
GpioRp1Pin RX_PIN;
GpioRp1PinMode TX_ALT;
GpioRp1PinMode RX_ALT;
Rp1Irq IRQ;
}uart_info_t;

const uart_info_t UART_DEVICES[UART_COUNT]=
	{
	{ RP1_UART0_BASE, GpioRp1Pin::Gpio14, GpioRp1Pin::Gpio15, GpioRp1PinMode::Alt4, GpioRp1PinMode::Alt4, Rp1Irq::UArt0 },
	{ RP1_UART1_BASE, GpioRp1Pin::Gpio0, GpioRp1Pin::Gpio1, GpioRp1PinMode::Alt2, GpioRp1PinMode::Alt2, Rp1Irq::UArt1 },
	{ RP1_UART2_BASE, GpioRp1Pin::Gpio4, GpioRp1Pin::Gpio5, GpioRp1PinMode::Alt2, GpioRp1PinMode::Alt2, Rp1Irq::UArt2 },
	{ RP1_UART3_BASE, GpioRp1Pin::Gpio8, GpioRp1Pin::Gpio9, GpioRp1PinMode::Alt2, GpioRp1PinMode::Alt2, Rp1Irq::UArt3 },
	{ RP1_UART4_BASE, GpioRp1Pin::Gpio12, GpioRp1Pin::Gpio13, GpioRp1PinMode::Alt2, GpioRp1PinMode::Alt2, Rp1Irq::UArt4 }
	};


//===========
// Registers
//===========

typedef struct
{
rw32_t DATA;
rw32_t STATUS;
rw32_t RES0[4];
rw32_t FLAGS;
rw32_t RES1;
rw32_t ILPR;
rw32_t IBRD;
rw32_t FBRD;
rw32_t LCRH;
rw32_t CTRL;
rw32_t IFLS;
rw32_t IMSC;
rw32_t RIS;
rw32_t MIS;
rw32_t ICR;
rw32_t DMACR;
}pl011_regs_t;

constexpr uint32_t CTRL_RX_ENABLE		=(1<<9);
constexpr uint32_t CTRL_TX_ENABLE		=(1<<8);
constexpr uint32_t CTRL_ENABLE			=(1<<0);

constexpr uint32_t FLAG_TX_EMPTY		=(1<<7);
constexpr uint32_t FLAG_RX_FULL			=(1<<6);
constexpr uint32_t FLAG_TX_FULL			=(1<<5);
constexpr uint32_t FLAG_RX_EMPTY		=(1<<4);
constexpr uint32_t FLAG_BUSY			=(1<<3);

constexpr bits32_t IFLS_RXIFSEL			={ 7, 3 };
constexpr bits32_t IFLS_TXIFSEL			={ 7, 0 };
constexpr uint32_t IFLS_IFSEL_1_8		=0;
constexpr uint32_t IFLS_IFSEL_1_4		=1;
constexpr uint32_t IFLS_IFSEL_1_2		=2;
constexpr uint32_t IFLS_IFSEL_3_4		=3;
constexpr uint32_t IFLS_IFSEL_7_8		=4;

constexpr uint32_t IMSC_INT_OE			=(1<<10);
constexpr uint32_t IMSC_INT_RT			=(1<<6);
constexpr uint32_t IMSC_INT_TX			=(1<<5);
constexpr uint32_t IMSC_INT_RX			=(1<<4);

constexpr uint32_t LCRH_WORD_LEN_8		=3<<5;
constexpr uint32_t LCRH_FIFO_ENABLE		=1<<4;


//===========
// Baud-Rate
//===========

constexpr UINT BAUD_INT(UINT clock, UINT baud)
{
return clock/(baud<<4);
}

constexpr UINT BAUD_FRAC(UINT clock, UINT baud)
{
UINT baud16=baud<<4;
UINT baud_int=clock/baud16;
UINT baud_frac=(clock%baud16)*8/baud;
return baud_frac/2+baud_frac%2;
}


//==================
// Con-/Destructors
//==================

SerialPort::~SerialPort()
{
WriteLock lock(s_Mutex);
if(m_PcieHost)
	m_PcieHost->SetInterruptHandler(UART_DEVICES[m_Id].IRQ, nullptr);
if(m_ServiceTask)
	m_ServiceTask->Cancel();
s_Current[m_Id]=nullptr;
}

Handle<SerialPort> SerialPort::Create(SerialDevice device, BaudRate baud)
{
UINT id=(UINT)device;
WriteLock lock(s_Mutex);
if(s_Current[id])
	throw AccessDeniedException();
auto serial=(SerialPort*)operator new(sizeof(SerialPort));
try
	{
	new (serial) SerialPort(device, baud);
	}
catch(Exception e)
	{
	delete serial;
	throw e;
	}
s_Current[id]=serial;
return serial;
}


//==============
// Input-Stream
//==============

SIZE_T SerialPort::Available()
{
SpinLock lock(m_CriticalSection);
return m_InputBuffer->Available();
}

SIZE_T SerialPort::Read(VOID* buf, SIZE_T size)
{
SpinLock lock(m_CriticalSection);
return m_InputBuffer->Read(buf, size);
}


//===============
// Output-Stream
//===============

VOID SerialPort::Flush()
{
m_WriteBuffer->Flush();
m_Signal.Trigger();
}

SIZE_T SerialPort::Write(VOID const* buf, SIZE_T size)
{
return m_WriteBuffer->Write(buf, size);
}


//==========================
// Con-/Destructors Private
//==========================

SerialPort::SerialPort(SerialDevice device, BaudRate baud):
m_BaudRate(baud),
m_Device((VOID*)UART_DEVICES[(UINT)device].BASE),
m_Id((UINT)device)
{
m_InputBuffer=RingBuffer::Create(UART_INPUT_RING);
m_WriteBuffer=WriteBuffer::Create();
auto name=String::Create("serial%u", m_Id);
m_ServiceTask=ServiceTask::Create(this, &SerialPort::ServiceTask, name);
}

SerialPort* SerialPort::s_Current[UART_COUNT]={ nullptr };
Mutex SerialPort::s_Mutex;


//================
// Common Private
//================

VOID SerialPort::HandleInterrupt(VOID* param)
{
auto serial=(SerialPort*)param;
serial->OnInterrupt();
}

VOID SerialPort::OnInterrupt()
{
SpinLock lock(m_CriticalSection);
auto uart=(pl011_regs_t*)m_Device;
Status status=Status::Success;
while(!io_read(uart->FLAGS, FLAG_RX_EMPTY))
	{
	UINT value=io_read(uart->DATA);
	if(!m_InputBuffer->Write((BYTE)value))
		status=Status::BufferOverrun;
	}
while(m_WriteBuffer->Available())
	{
	if(io_read(uart->FLAGS, FLAG_TX_FULL))
		break;
	BYTE value=0;
	m_WriteBuffer->Read(&value, 1);
	io_write(uart->DATA, value);
	}
UINT mis=io_read(uart->MIS);
io_write(uart->ICR, mis); // ACK
m_Signal.Trigger(status);
}

VOID SerialPort::ServiceTask()
{
m_GpioHost=GpioHost::Get();
m_GpioHost->SetPinMode(UART_DEVICES[m_Id].TX_PIN, UART_DEVICES[m_Id].TX_ALT);
m_GpioHost->SetPinMode(UART_DEVICES[m_Id].RX_PIN, UART_DEVICES[m_Id].RX_ALT, GpioPullMode::PullUp);
m_PcieHost=PcieHost::Get();
m_PcieHost->SetInterruptHandler(UART_DEVICES[m_Id].IRQ, HandleInterrupt, this);
auto uart=(pl011_regs_t*)m_Device;
if(io_read(uart->FLAGS, FLAG_BUSY))
	throw DeviceNotReadyException();
io_write(uart->IMSC, 0);
io_write(uart->ICR, 0x7FF);
io_write(uart->IBRD, BAUD_INT(UART_CLOCK, (UINT)m_BaudRate));
io_write(uart->FBRD, BAUD_FRAC(UART_CLOCK, (UINT)m_BaudRate));
io_write(uart->LCRH, LCRH_WORD_LEN_8|LCRH_FIFO_ENABLE);
UINT ifls=0;
bits_set(ifls, IFLS_RXIFSEL, IFLS_IFSEL_3_4);
bits_set(ifls, IFLS_TXIFSEL, IFLS_IFSEL_1_4);
io_write(uart->IFLS, ifls);
io_write(uart->IMSC, IMSC_INT_OE|IMSC_INT_RT|IMSC_INT_TX|IMSC_INT_RX);
io_set(uart->CTRL, CTRL_RX_ENABLE|CTRL_TX_ENABLE|CTRL_ENABLE);
auto task=Task::Get();
SpinLock spin_lock(m_CriticalSection);
while(!task->Cancelled)
	{
	UINT read=m_InputBuffer->Available();
	if(read)
		{
		spin_lock.Unlock();
		DataReceived(this);
		spin_lock.Lock();
		}
	while(m_WriteBuffer->Available())
		{
		if(io_read(uart->FLAGS, FLAG_TX_FULL))
			break;
		BYTE value=0;
		m_WriteBuffer->Read(&value, 1);
		io_write(uart->DATA, value);
		}
	if(!read)
		m_Signal.Wait(spin_lock);
	}
}

}}