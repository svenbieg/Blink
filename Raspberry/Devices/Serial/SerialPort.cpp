//================
// SerialPort.cpp
//================

#include "SerialPort.h"


//=======
// Using
//=======

using namespace Concurrency;
using namespace Devices::Gpio;
using namespace Devices::System;
using namespace Storage;


//===========
// Namespace
//===========

namespace Devices {
	namespace Serial {


//==========
// Settings
//==========

const UINT PL011_RX_BYTES=48;
const UINT PL011_TX_BYTES=32;

const UINT UART_RX_BYTES=PL011_RX_BYTES*4;
const UINT UART_TX_BYTES=PL011_TX_BYTES*4;


//===========
// Registers
//===========

typedef struct
{
RW32 DATA;
RW32 STATUS;
RW32 RES0[4];
RW32 FLAGS;
RW32 RES1;
RW32 ILPR;
RW32 IBRD;
RW32 FBRD;
RW32 LCRH;
RW32 CTRL;
RW32 IFLS;
RW32 IMSC;
RW32 RIS;
RW32 MIS;
RW32 ICR;
RW32 DMACR;
}PL011_REGS;

const UINT CTRL_RX_ENABLE		=(1<<9);
const UINT CTRL_TX_ENABLE		=(1<<8);
const UINT CTRL_ENABLE			=(1<<0);

const UINT FLAG_TX_EMPTY		=(1<<7);
const UINT FLAG_RX_FULL			=(1<<6);
const UINT FLAG_TX_FULL			=(1<<5);
const UINT FLAG_RX_EMPTY		=(1<<4);
const UINT FLAG_BUSY			=(1<<3);

const BITS IFLS_RXIFSEL			={ 7, 3 };
const BITS IFLS_TXIFSEL			={ 7, 0 };
const UINT IFLS_IFSEL_1_8		=0;
const UINT IFLS_IFSEL_1_4		=1;
const UINT IFLS_IFSEL_1_2		=2;
const UINT IFLS_IFSEL_3_4		=3;
const UINT IFLS_IFSEL_7_8		=4;

const UINT IMSC_INT_OE			=(1<<10);
const UINT IMSC_INT_RT			=(1<<6);
const UINT IMSC_INT_TX			=(1<<5);
const UINT IMSC_INT_RX			=(1<<4);

const UINT LCRH_WORD_LEN_8		=(3<<5);
const UINT LCRH_FIFO_ENABLE		=(1<<4);


//===========
// Baud-Rate
//===========

inline UINT BAUD_INT(UINT clock, UINT baud)
{
return clock/(baud<<4);
}

inline UINT BAUD_FRAC(UINT clock, UINT baud)
{
UINT baud16=baud<<4;
UINT rem=clock%baud16;
return (rem*64+baud16/2)/baud16;
}


//==================
// Con-/Destructors
//==================

SerialPort::~SerialPort()
{
m_ServiceTask->Cancel();
}

Handle<SerialPort> SerialPort::Create(SerialDevice device, BaudRate baud)
{
WriteLock lock(s_Mutex);
UINT id=(UINT)device;
if(s_Current[id])
	throw AccessDeniedException();
auto serial=Object::Create<SerialPort>(device, baud);
s_Current[id]=serial;
return serial;
}


//==============
// Input-Stream
//==============

SIZE_T SerialPort::Available()
{
return m_InputBuffer->Available();
}

SIZE_T SerialPort::Read(VOID* buf, SIZE_T size)
{
auto dst=(BYTE*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	SIZE_T available=m_InputBuffer->Available();
	if(!available)
		{
		m_InputSignal.Wait();
		continue;
		}
	SIZE_T copy=TypeHelper::Min(available, size-pos);
	SIZE_T read=m_InputBuffer->Read(&dst[pos], copy);
	pos+=read;
	}
return pos;
}


//===============
// Output-Stream
//===============

VOID SerialPort::Flush()
{
m_OutputBuffer->Flush();
m_OutputSignal.Trigger();
}

SIZE_T SerialPort::Write(VOID const* buf, SIZE_T size)
{
return m_OutputBuffer->Write(buf, size);
}


//==========================
// Con-/Destructors Private
//==========================

SerialPort::SerialPort(SerialDevice device, BaudRate baud):
m_BaudRate(baud),
m_Device((VOID*)SERIAL_DEVICES[(UINT)device].BASE),
m_Id((UINT)device)
{
m_InputBuffer=RingBuffer::Create(UART_RX_BYTES);
m_OutputBuffer=StreamBuffer::Create(UART_TX_BYTES);
auto name=String::Create("serial%u", m_Id);
m_ServiceTask=ServiceTask::Create(this, &SerialPort::ServiceTask, name, SERIAL_STACK);
}

SerialPort* SerialPort::s_Current[SERIAL_COUNT]={ nullptr };
Mutex SerialPort::s_Mutex;


//==================
// Common Protected
//==================

UINT SerialPort::Release()noexcept
{
WriteLock lock(s_Mutex);
UINT ref_count=Cpu::InterlockedDecrement(&m_ReferenceCount);
if(ref_count==0)
	{
	s_Current[m_Id]=nullptr;
	delete this;
	}
return ref_count;
}


//================
// Common Private
//================

VOID SerialPort::OnInterrupt()
{
SpinLock lock(m_CriticalSection);
auto uart=(PL011_REGS*)m_Device;
Status status=Status::Success;
SIZE_T read=0;
while(!IoHelper::Read(uart->FLAGS, FLAG_RX_EMPTY))
	{
	UINT value=IoHelper::Read(uart->DATA);
	if(!m_InputBuffer->Write((BYTE)value))
		{
		status=Status::BufferOverrun;
		break;
		}
	read++;
	}
if(read)
	m_InputSignal.Trigger();
SIZE_T written=0;
while(m_OutputBuffer->Available())
	{
	if(IoHelper::Read(uart->FLAGS, FLAG_TX_FULL))
		break;
	BYTE value=0;
	m_OutputBuffer->Read(&value, 1);
	IoHelper::Write(uart->DATA, value);
	written++;
	}
if(written)
	m_OutputSignal.Trigger(status);
UINT mis=IoHelper::Read(uart->MIS);
IoHelper::Write(uart->ICR, mis); // ACK
}

VOID SerialPort::ServiceTask()
{
auto reset=SERIAL_DEVICES[m_Id].RESET;
System::System::Enable(reset);
GpioHelper::SetPinMode(SERIAL_DEVICES[m_Id].TX_PIN, SERIAL_DEVICES[m_Id].TX_ALT);
GpioHelper::SetPinMode(SERIAL_DEVICES[m_Id].RX_PIN, SERIAL_DEVICES[m_Id].RX_ALT, GpioPullMode::PullUp);
Interrupts::SetHandler(SERIAL_DEVICES[m_Id].IRQ, this, &SerialPort::OnInterrupt);
auto uart=(PL011_REGS*)m_Device;
if(IoHelper::Read(uart->FLAGS, FLAG_BUSY))
	throw DeviceNotReadyException();
IoHelper::Write(uart->IMSC, 0);
IoHelper::Write(uart->ICR, 0x7FF);
IoHelper::Write(uart->IBRD, BAUD_INT(SERIAL_CLOCK, (UINT)m_BaudRate));
IoHelper::Write(uart->FBRD, BAUD_FRAC(SERIAL_CLOCK, (UINT)m_BaudRate));
IoHelper::Write(uart->LCRH, LCRH_WORD_LEN_8|LCRH_FIFO_ENABLE);
UINT ifls=0;
BitHelper::Set(ifls, IFLS_RXIFSEL, IFLS_IFSEL_3_4);
BitHelper::Set(ifls, IFLS_TXIFSEL, IFLS_IFSEL_1_4);
IoHelper::Write(uart->IFLS, ifls);
IoHelper::Write(uart->IMSC, IMSC_INT_OE|IMSC_INT_RT|IMSC_INT_TX|IMSC_INT_RX);
IoHelper::Set(uart->CTRL, CTRL_RX_ENABLE|CTRL_TX_ENABLE|CTRL_ENABLE);
auto task=Task::Get();
SpinLock lock(m_CriticalSection);
while(!task->Cancelled)
	{
	while(m_OutputBuffer->Available())
		{
		if(IoHelper::Read(uart->FLAGS, FLAG_TX_FULL))
			break;
		BYTE value=0;
		m_OutputBuffer->Read(&value, 1);
		IoHelper::Write(uart->DATA, value);
		}
	m_OutputSignal.Wait(lock);
	}
}

}}