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


//==========
// Settings
//==========

const DmaMode DMA_MODE[]={ DmaMode::Bits8, DmaMode::Bits16, DmaMode::Bits32 };
const BYTE SPI_DATA_BITS[]={ 8, 16, 32 };
const UINT SPI_DATA_SIZE[]={ 1, 2, 4 };


//=========
// Program
//=========

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

SpiEmulator::SpiEmulator(SPI_CONFIG const& config):
m_DataSize(SPI_DATA_SIZE[(UINT)config.Mode]),
m_InputBuffer(nullptr),
m_InputRequest(DmaRequest::Force),
m_PinChipSelect(config.PinChipSelect),
m_OutputBuffer(nullptr),
m_OutputRequest(DmaRequest::Force)
{
m_StateMachine=StateMachine::Create(SPI_PROGRAM);
m_PinMode=m_StateMachine->GetPinMode();
m_GpioHost=GpioHost::Create();
m_GpioHost->SetPinMode(config.PinClock, m_PinMode, GpioPullMode::PullDown);
m_GpioHost->SetPinMode(config.PinClock, GpioStrength::_12mA);
m_GpioHost->SetPinMode(config.PinClock, GpioSlew::Fast);
m_GpioHost->SetPinMode(config.PinTx, m_PinMode, GpioPullMode::PullDown);
if(config.PinTx!=config.PinRx)
	m_GpioHost->SetPinMode(config.PinRx, m_PinMode);
m_GpioHost->SetPinMode(config.PinRx, GpioHysteresis::SchmittTrigger);
m_GpioHost->SetPinMode(config.PinChipSelect, GpioPinMode::Output);
m_GpioHost->DigitalWrite(config.PinChipSelect, true);
auto data_bits=SPI_DATA_BITS[(UINT)config.Mode];
m_StateMachine->SetDivisor(config.Divisor);
m_StateMachine->SetInputPin(config.PinRx);
m_StateMachine->SetInputShift(false, true, data_bits);
m_StateMachine->SetInputSyncBypass(config.PinRx);
m_StateMachine->SetOutputPins(config.PinTx, 1);
m_StateMachine->SetOutputShift(false, true, data_bits);
m_StateMachine->SetSetPins(config.PinRx, 1);
m_StateMachine->SideSet(config.PinClock, 1, false, false);
m_StateMachine->Wrap(0, 5);
m_StateMachine->SetPinDirection(config.PinClock, PioPinDirection::Output);
m_StateMachine->SetPinDirection(config.PinTx, PioPinDirection::Output);
if(config.PinTx!=config.PinRx)
	m_StateMachine->SetPinDirection(config.PinRx, PioPinDirection::Input);
m_StateMachine->SetPins(0);
auto dma_mode=DMA_MODE[(UINT)config.Mode];
m_DmaChannel=DmaChannel::Create();
m_DmaChannel->SetByteSwap(true);
m_DmaChannel->SetMode(dma_mode);
m_InputBuffer=m_StateMachine->GetInputBuffer();
m_InputRequest=m_StateMachine->GetInputRequest();
m_OutputBuffer=m_StateMachine->GetOutputBuffer();
m_OutputRequest=m_StateMachine->GetOutputRequest();
}


//==================
// Common Protected
//==================

VOID SpiEmulator::SpiBegin(UINT tx_size, UINT rx_size)
{
assert(tx_size%m_DataSize==0);
assert(rx_size%m_DataSize==0);
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
m_DmaChannel->BeginRead(m_InputRequest, m_InputBuffer, buf, size);
m_DmaChannel->Wait();
}

VOID SpiEmulator::SpiWrite(VOID const* buf, UINT size)
{
m_DmaChannel->BeginWrite(m_OutputRequest, m_OutputBuffer, buf, size);
m_DmaChannel->Wait();
}

}}