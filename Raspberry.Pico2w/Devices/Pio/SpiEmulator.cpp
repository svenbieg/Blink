//=================
// SpiEmulator.cpp
//=================

#include "SpiEmulator.h"


//=======
// Using
//=======

using namespace Devices::Dma;
using namespace Devices::Gpio;
using namespace Devices::Pio;


//===========
// Namespace
//===========

namespace Devices {
	namespace Pio {


//=====
// PIO
//=====

const WORD SPI_INSTR[]=
{
0x6001, // 0: out pins, 1		| side 0
0x1040, // 1: jmp x--, 0		| side 1
0xA042, // 2: nop				| side 0
0xB042, // 3: nop				| side 1
0x4001, // 4: in pins, 1		| side 0
0x1084, // 5: jmp y--, 4		| side 1
};

const PIO_PROGRAM SPI_PROGRAM={ SPI_INSTR, TypeHelper::ArraySize(SPI_INSTR) };


//============================
// Con-/Destructors Protected
//============================

SpiEmulator::SpiEmulator(SpiConfiguration const& config):
m_InputBuffer(nullptr),
m_InputRequest(DmaRequest::Force),
m_PinChipSelect(config.PinChipSelect),
m_OutputBuffer(nullptr),
m_OutputRequest(DmaRequest::Force)
{
m_StateMachine=StateMachine::Create(SPI_PROGRAM);
auto gpio_mode=m_StateMachine->GetPinMode();
m_GpioHost=GpioHost::Create();
m_GpioHost->SetPinMode(config.PinClock, gpio_mode, GpioPullMode::PullDown);
m_GpioHost->SetPinMode(config.PinClock, GpioStrength::_12mA);
m_GpioHost->SetPinMode(config.PinClock, GpioSlew::Fast);
m_GpioHost->SetPinMode(config.PinTx, gpio_mode, GpioPullMode::PullDown);
m_GpioHost->SetPinMode(config.PinRx, gpio_mode);
m_GpioHost->SetPinMode(config.PinRx, GpioHysteresis::SchmittTrigger);
m_GpioHost->SetPinMode(config.PinChipSelect, GpioPinMode::Output);
m_GpioHost->DigitalWrite(config.PinChipSelect, true);
m_StateMachine->SetDivisor(config.Divisor);
m_StateMachine->SetInputPin(config.PinRx);
m_StateMachine->SetInputShift(false, true, 16);
m_StateMachine->SetInputSyncBypass(config.PinRx);
m_StateMachine->SetOutputPins(config.PinTx, 1);
m_StateMachine->SetOutputShift(false, true, 16);
m_StateMachine->SideSet(config.PinClock, 1, false, false);
m_StateMachine->Wrap(0, 5);
m_StateMachine->SetPinDirection(config.PinClock, PioPinDirection::Output);
m_StateMachine->SetPinDirection(config.PinTx, PioPinDirection::Output);
m_StateMachine->SetPinDirection(config.PinRx, PioPinDirection::Input);
m_StateMachine->SetPins(0);
m_InputBuffer=m_StateMachine->GetInputBuffer();
m_InputDma=DmaChannel::Create();
m_InputDma->SetByteSwap(true);
m_InputDma->SetDataSize(DmaDataSize::Bits16);
m_InputRequest=m_StateMachine->GetInputRequest();
m_OutputBuffer=m_StateMachine->GetOutputBuffer();
m_OutputDma=DmaChannel::Create();
m_OutputDma->SetByteSwap(true);
m_OutputDma->SetDataSize(DmaDataSize::Bits16);
m_OutputRequest=m_StateMachine->GetOutputRequest();
}


//==================
// Common Protected
//==================

VOID SpiEmulator::SpiBegin(UINT tx_size, UINT rx_size)
{
m_GpioHost->DigitalWrite(m_PinChipSelect, false);
m_StateMachine->Restart();
m_StateMachine->ClearBuffers();
m_StateMachine->Output(PioOutput::X, tx_size*8-1, 32);
m_StateMachine->Output(PioOutput::Y, rx_size*8-1, 32);
m_StateMachine->Jump(0);
m_StateMachine->Start();
}

VOID SpiEmulator::SpiEnd()
{
m_GpioHost->DigitalWrite(m_PinChipSelect, true);
m_StateMachine->Stop();
m_StateMachine->SetPins(0);
}

VOID SpiEmulator::SpiRead(VOID* buf, UINT size)
{
m_InputDma->BeginRead(m_InputRequest, m_InputBuffer, buf, size);
m_InputDma->Wait();
}

VOID SpiEmulator::SpiWrite(VOID const* buf, UINT size)
{
m_OutputDma->BeginWrite(m_OutputRequest, m_OutputBuffer, buf, size);
m_OutputDma->Wait();
}

}}