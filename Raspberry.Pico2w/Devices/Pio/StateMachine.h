//================
// StateMachine.h
//================

#pragma once


//=======
// Using
//=======

#include "Concurrency/Mutex.h"
#include "Devices/Dma/DmaChannel.h"
#include "Devices/Gpio/GpioHost.h"
#include "Devices/System/System.h"
#include "Object.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Pio {


//=========
// Program
//=========

typedef struct
{
WORD const* Instructions;
WORD InstructionCount;
}PIO_PROGRAM;


//===============
// Pin-Direction
//===============

enum class PioPinDirection
{
Input,
Output
};


//========
// Output
//========

enum class PioOutput
{
X=1,
Y=2
};


//===============
// State-Machine
//===============

class StateMachine: public Object
{
public:
	// Using
	using DmaRequest=Devices::Dma::DmaRequest;
	using GpioPin=Devices::Gpio::GpioPin;
	using GpioPinMode=Devices::Gpio::GpioPinMode;
	using Mutex=Concurrency::Mutex;
	using ResetDevice=Devices::System::ResetDevice;

	// Con-/Destructors
	~StateMachine();
	static Handle<StateMachine> Create(PIO_PROGRAM const& Program);

	// Common
	VOID ClearBuffers();
	RO32* GetInputBuffer();
	DmaRequest GetInputRequest();
	RW32* GetOutputBuffer();
	DmaRequest GetOutputRequest();
	GpioPinMode GetPinMode();
	VOID Jump(WORD Address);
	VOID Output(PioOutput Destination, UINT Buffer, UINT BitCount);
	VOID Restart();
	VOID SetDivisor(BYTE Divisor, BYTE Fraction=0);
	VOID SetInputPin(GpioPin Pin);
	VOID SetInputShift(BOOL ShiftRight, BOOL AutoPush, UINT PushTreshold);
	VOID SetInputSyncBypass(GpioPin Pin);
	VOID SetOutputPins(GpioPin Pin, BYTE Count);
	VOID SetOutputShift(BOOL ShiftRight, BOOL AutoPull, UINT PullTreshold);
	VOID SetPinDirection(GpioPin Pin, PioPinDirection Direction);
	VOID SetPins(BOOL Value);
	VOID SetSetPins(GpioPin Pin, BYTE Count);
	VOID SideSet(GpioPin Pin, BYTE BitCount, BOOL Optional, BOOL Directional);
	VOID Start();
	VOID Stop();
	VOID Wrap(WORD WrapTarget, WORD Wrap);

private:
	// Con-/Destructors
	StateMachine(BYTE Pio, BYTE StateMachine, UINT InstructionsUsed);

	// Common
	static UINT GetStateMachine(UINT Pio)noexcept;
	static VOID Reset(UINT Pio);
	static VOID UploadProgram(UINT Pio, UINT Offset, PIO_PROGRAM const& Program);
	UINT m_InstructionsUsed;
	BYTE m_Pio;
	BYTE m_StateMachine;
};

}}