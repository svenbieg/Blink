//=============
// WifiSpi.cpp
//=============

#include "WifiSpi.h"


//=======
// Using
//=======

using namespace Devices::Gpio;
using namespace Devices::Pio;
using namespace Devices::Sdio;


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//=====
// PIO
//=====

const WORD SPI_INSTR[]=
{
0x6001, // 0: out pins, 1		| side 0
0x1040, // 1: jmp x--, 0		| side 1
0xE080, // 2: set pindirs, 0	| side 0
0xB042, // 3: nop				| side 1
0x4001, // 4: in pins, 1		| side 0
0x1084, // 5: jmp y--, 4		| side 1
};

const PIO_PROGRAM SPI_PROGRAM={ SPI_INSTR, TypeHelper::ArraySize(SPI_INSTR) };


//============================
// Con-/Destructors Protected
//============================

WifiSpi::WifiSpi():
m_InputBuffer(nullptr),
m_InputRequest(DmaRequest::Force),
m_OutputBuffer(nullptr),
m_OutputRequest(DmaRequest::Force)
{
m_StateMachine=StateMachine::Create(SPI_PROGRAM);
auto gpio_mode=m_StateMachine->GetPinMode();
m_GpioHost=GpioHost::Create();
m_GpioHost->SetPinMode(PIN_CLOCK, gpio_mode, GpioPullMode::PullDown);
m_GpioHost->SetPinMode(PIN_CLOCK, GpioStrength::_12mA);
m_GpioHost->SetPinMode(PIN_CLOCK, GpioSlew::Fast);
m_GpioHost->SetPinMode(PIN_DATA, gpio_mode, GpioPullMode::PullDown);
m_GpioHost->SetPinMode(PIN_DATA, GpioHysteresis::SchmittTrigger);
m_GpioHost->SetPinMode(PIN_CHIPSEL, GpioPinMode::Output);
m_GpioHost->DigitalWrite(PIN_CHIPSEL, true);
m_StateMachine->SetDivisor(2);
m_StateMachine->SetInputPin(PIN_DATA);
m_StateMachine->SetInputShift(false, true, 32);
m_StateMachine->SetInputSyncBypass(PIN_DATA);
m_StateMachine->SetOutputPins(PIN_DATA, 1);
m_StateMachine->SetOutputShift(false, true, 32);
m_StateMachine->SetSetPins(PIN_DATA, 1);
m_StateMachine->SideSet(PIN_CLOCK, 1, false, false);
m_StateMachine->Wrap(0, 5);
m_StateMachine->SetPinDirection(PIN_CLOCK, PioPinDirection::Output);
m_StateMachine->SetPinDirection(PIN_DATA, PioPinDirection::Output);
m_StateMachine->SetPins(0);
m_InputBuffer=m_StateMachine->GetInputBuffer();
m_InputDma=DmaChannel::Create();
m_InputDma->SetByteSwap(true);
m_InputRequest=m_StateMachine->GetInputRequest();
m_OutputBuffer=m_StateMachine->GetOutputBuffer();
m_OutputDma=DmaChannel::Create();
m_OutputDma->SetByteSwap(true);
m_OutputRequest=m_StateMachine->GetOutputRequest();
}


//==================
// Common Protected
//==================

VOID WifiSpi::SpiBegin(UINT tx_count, UINT rx_count)
{
m_GpioHost->DigitalWrite(PIN_CHIPSEL, false);
auto gpio_mode=m_StateMachine->GetPinMode();
m_GpioHost->SetPinMode(PIN_DATA, gpio_mode);
m_StateMachine->Restart();
m_StateMachine->ClearBuffers();
m_StateMachine->SetPinDirection(PIN_DATA, PioPinDirection::Output);
m_StateMachine->Output(PioOutput::X, tx_count*32-1, 32);
m_StateMachine->Output(PioOutput::Y, rx_count*32-1, 32);
m_StateMachine->Jump(0);
m_StateMachine->Start();
}

VOID WifiSpi::SpiEnd()
{
m_GpioHost->DigitalWrite(PIN_CHIPSEL, true);
m_StateMachine->Stop();
m_StateMachine->SetPins(0);
m_GpioHost->SetPinMode(PIN_DATA, GpioPinMode::Input, GpioPullMode::PullDown);
}

VOID WifiSpi::SpiRead(UINT* buf, UINT count)
{
m_InputDma->BeginRead(m_InputRequest, m_InputBuffer, buf, count);
m_InputDma->Wait();
}

VOID WifiSpi::SpiWrite(UINT const* buf, UINT count)
{
m_OutputDma->BeginWrite(m_OutputRequest, m_OutputBuffer, buf, count);
m_OutputDma->Wait();
}

}}