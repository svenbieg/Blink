//==============
// SerialPort.h
//==============

#pragma once


//=======
// Using
//=======

#include "Devices/Gpio/GpioHost.h"
#include "Devices/Pcie/PcieHost.h"
#include "Storage/Streams/RandomAccessStream.h"
#include "Storage/Streams/OutputBuffer.h"
#include "Storage/RingBuffer.h"
#include "Event.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Serial {


//=========
// Devices
//=========

constexpr UINT UART_COUNT=5;

enum class SerialDevice
{
Serial0,
Serial1,
Serial2,
Serial3,
Serial4
};


//===========
// Baud-Rate
//===========

enum class BaudRate: UINT
{
Baud57600=57600,
Baud74880=74880,
Baud115200=115200,
Baud230400=230400,
Baud460800=460800,
Baud921600=921600
};


//=============
// Serial-Port
//=============

class SerialPort: public Storage::Streams::RandomAccessStream
{
public:
	// Using
	using GpioHost=Devices::Gpio::GpioHost;
	using OutputBuffer=Storage::Streams::OutputBuffer;
	using PcieHost=Devices::Pcie::PcieHost;
	using RingBuffer=Storage::RingBuffer;

	// Con-/Destructors
	~SerialPort();
	static Handle<SerialPort> Create(SerialDevice Device=SerialDevice::Serial0, BaudRate Baud=BaudRate::Baud115200);

	// Common
	Event<SerialPort> DataReceived;

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Con-/Destructors
	SerialPort(SerialDevice Device, BaudRate Baud);
	static SerialPort* s_Current[UART_COUNT];
	static Concurrency::Mutex s_Mutex;

	// Common
	static VOID HandleInterrupt(VOID* Parameter);
	VOID OnInterrupt();
	VOID ServiceTask();
	BaudRate m_BaudRate;
	Concurrency::CriticalSection m_CriticalSection;
	VOID* m_Device;
	Handle<GpioHost> m_GpioHost;
	UINT m_Id;
	Handle<RingBuffer> m_InputBuffer;
	Handle<PcieHost> m_PcieHost;
	Handle<Concurrency::Task> m_ServiceTask;
	Concurrency::Signal m_Signal;
	Handle<OutputBuffer> m_OutputBuffer;
};

}}