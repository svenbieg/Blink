//==============
// SerialPort.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/RandomAccessStream.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Serial {


//=========
// Devices
//=========

enum class UArtDevice
{
UArt0,
UArt1,
UArt2,
UArt3,
UArt4,
UArt5,
UArt10=10
};

constexpr UArtDevice UArtDeviceDefault=UArtDevice::UArt0;


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

constexpr BaudRate BaudRateDefault=BaudRate::Baud115200;


//=============
// Serial-Port
//=============

class SerialPort: public Storage::Streams::RandomAccessStream
{
public:
	// Common
	static Handle<SerialPort> Open(UArtDevice Device=UArtDeviceDefault, BaudRate Baud=BaudRateDefault);

	// Serial-Port
	BOOL Poll();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Con-/Destructors
	SerialPort(UArtDevice Device, BaudRate Baud);

	// Common
	BYTE Read();
	VOID Write(BYTE Value);
	SIZE_T m_Address;
	BaudRate m_BaudRate;
	UArtDevice m_Device;
};

}}
