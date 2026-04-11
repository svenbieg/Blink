//===========
// MailBox.h
//===========

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//==========
// Channels
//==========

enum class MailBoxChannel: BYTE
{
Power,
FrameBuffer,
VirtualUART,
VirtualCHIQ,
Leds,
Buttons,
TouchScreen,
Counter,
Properties
};


//==============
// Property-Tag
//==============

enum class PropertyTag: UINT
{
GetFirmwareRevision				=0x1,
SetCursorInfo					=0x8010,
SetCursorState,
GetBoardModel					=0x10001,
GetBoardRevision,
GetBoardMAC,
GetBoardSerial,
GetArmMemory,
GetVideoCoreMemory,
GetClocks,
GetPowerState					=0x20001,
GetTiming,
SetPowerState					=0x28001,
GetClockState					=0x30001,
GetClockRate,
GetVoltage,
GetMaxClockRate,
GetMaxVoltage,
GetTemperature,
GetMinClockRate,
GetMinVoltage,
GetTurbo,
GetMaxTemperature,
AllocateMemory					=0x3000C,
LockMemory,
UnlockMemory,
ReleaseMemory,
ExecuteCode,
GetDispManResMemHandle			=0x30014,
GetDomainState					=0x30030,
SetClockState					=0x38001,
SetClockRate,
SetVoltage,
SetTurbo						=0x38009,
SetDomainState					=0x38030,
GetGpioState					=0x30041,
GetGpioConfig					=0x30043,
GetPeripheralsReg				=0x30045,
SetGpioState					=0x38041,
SetSdHostClock,
SetGpioConfig,
SetPeripheralsReg				=0x38045,
FrameBufAllocate				=0x40001,
FrameBufBlank,
FrameBufGetPhysWidthHeight,
FrameBufGetVirtWidthHeight,
FrameBufGetDepth,
FrameBufGetPixelOrder,
FrameBufGetAlphaMode,
FrameBufGetPitch,
FrameBufGetVirtualOffset,
FrameBufGetOverscan,
FrameBufGetPalette,
FrameBufGetTouchBuf				=0x4000F,
FrameBufGetGpioVirtBuf,
FrameBufRelease					=0x48001,
FrameBufTestPhysWidthHeight		=0x44003,
FrameBufTestVirtWidthHeight,
FrameBufTestDepth,
FrameBufTestPixelOrder,
FrameBufTestAlphaMode,
FrameBufTestVirtualOffset		=0x44009,
FrameBufTestOverscan,
FrameBufTestPalette,
FrameBufTestVSync				=0x4400E,
FrameBufSetPhysWidthHeight		=0x48003,
FrameBufSetVirtWidthHeight,
FrameBufSetDepth,
FrameBufSetPixelOrder,
FrameBufSetAlphaMode,
FrameBufSetVirtualOffset		=0x48009,
FrameBufSetOverscan,
FrameBufSetPalette,
FrameBufSetTouchBuffer			=0x4801F,
FrameBufSetGpioVirtBuf,
FrameBufSetVSync				=0x4800E,
FrameBufSetBacklight,
GetCommandLine					=0x50001,
GetDmaChannels					=0x60001
};


//============
// ARM-Memory
//============

typedef struct
{
UINT Offset;
UINT Size;
}ARM_MEMORY;


//============
// Clock-Type
//============

enum class ClockType: UINT
{
Emmc=1,
UArt=2,
Arm=3,
Core=4
};


//==========
// Mail-Box
//==========

class MailBox
{
public:
	// Common
	static VOID GetArmMemory(ARM_MEMORY* Info);
	static UINT GetClockRate(ClockType Clock);
	static BOOL GetProperty(PropertyTag Tag, VOID* Property, UINT Size);

private:
	// Common
	static BOOL Read(MailBoxChannel Channel, UINT Address);
	static VOID Write(MailBoxChannel Channel, UINT Address);
};

}}