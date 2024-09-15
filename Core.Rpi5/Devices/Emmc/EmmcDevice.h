//==============
// EmmcDevice.h
//==============

#pragma once


//===========
// Namespace
//===========

namespace Devices {
	namespace Emmc {


//========
// Status
//========

enum class EmmcStatus
{
Error,
Ready,
Reset
};


//=============
// Emmc-Device
//=============

class EmmcDevice: public Object
{
public:
	// Using
	using Mutex=Concurrency::Mutex;
	using Task=Concurrency::Task;

	// Con-/Destructors
	EmmcDevice(SIZE_T Address, UINT Interrupt);

	// Common
	inline EmmcStatus GetStatus()const { return uStatus; }
	VOID Initialize();
	Handle<Task> InitializeAsync();

private:
	// Common
	BOOL SetClockSpeed(UINT Hertz);
	Mutex cMutex;
	BYTE pDmaBuffer[PAGE_SIZE];
	SIZE_T uAddress;
	UINT uInterrupt;
	EmmcStatus uStatus;
};

}}
