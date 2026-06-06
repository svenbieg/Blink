//==============
// SerialPort.h
//==============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/Serial/SerialConfig.h"
#include "Storage/Streams/StreamBuffer.h"
#include "Storage/RingBuffer.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Serial {


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

class SerialPort: public Object, public Storage::Streams::RandomAccessStream
{
public:
	// Using
	using CriticalSection=Concurrency::CriticalSection;
	using RingBuffer=Storage::RingBuffer;
	using Signal=Concurrency::Signal;
	using StreamBuffer=Storage::Streams::StreamBuffer;
	using Task=Concurrency::Task;

	// Friends
	friend Object;

	// Con-/Destructors
	~SerialPort();
	static Handle<SerialPort> Create(SerialDevice Device=SerialDevice::Serial0, BaudRate Baud=BaudRate::Baud115200);

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

protected:
	// Common
	UINT Release()noexcept override;

private:
	// Con-/Destructors
	SerialPort(SerialDevice Device, BaudRate Baud);
	static SerialPort* s_Current[SERIAL_COUNT];
	static Concurrency::Mutex s_Mutex;

	// Common
	VOID OnInterrupt();
	VOID ServiceTask();
	BaudRate m_BaudRate;
	CriticalSection m_CriticalSection;
	VOID* m_Device;
	UINT m_Id;
	Handle<RingBuffer> m_InputBuffer;
	Signal m_InputSignal;
	Handle<StreamBuffer> m_OutputBuffer;
	Signal m_OutputSignal;
	Handle<Task> m_ServiceTask;
};

}}