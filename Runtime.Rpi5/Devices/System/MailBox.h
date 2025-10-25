//===========
// MailBox.h
//===========

#pragma once


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
Power=0,
FrameBuffer=1,
VirtualUART=2,
VirtualCHIQ=3,
Leds=4,
Buttons=5,
TouchScreen=6,
Counter=7,
Properties=8
};


//==============
// Property-Tag
//==============

enum class PropertyTag: UINT
{
GetFirmwareRevision=0x1,
SetCursorInfo=0x8010,
SetCursorState=0x8011,
GetBoardModel=0x10001,
GetBoardRevision=0x10002,
GetBoardMAC=0x10003,
GetBoardSerial=0x10004,
GetArmMemory=0x10005,
GetVideoCoreMemory=0x10006,
GetClocks=0x10007,
GetPowerState=0x20001,
GetTiming=0x20002,
SetPowerState=0x28001,
GetClockState=0x30001,
GetClockRate=0x30002,
GetVoltage=0x30003,
GetMaxClockRate=0x30004,
GetMaxVoltage=0x30005,
GetTemperature=0x30006,
GetMinClockRate=0x30007,
GetMinVoltage=0x30008,
GetTurbo=0x30009,
GetMaxTemperature=0x3000A,
AllocateMemory=0x3000C,
LockMemory=0x3000D,
UnlockMemory=0x3000E,
ReleaseMemory=0x3000F,
ExecuteCode=0x30010,
GetDisplayManagerResourceMemoryHandle=0x30014,
GetDomainState=0x30030,
SetClockState=0x38001,
SetClockRate=0x38002,
SetVoltage=0x38003,
SetTurbo=0x38009,
SetDomainState=0x38030,
GetGpioState=0x30041,
SetGpioState=0x38041,
SetSdHostClock=0x38042,
GetGpioConfig=0x30043,
SetGpioConfig=0x38043,
GetPeripheralsRegister=0x30045,
SetPeripheralsRegister=0x38045,
FrameBufferAllocate=0x40001,
FrameBufferBlank=0x40002,
FrameBufferGetPhysicalWidthHeight=0x40003,
FrameBufferGetVirtualWidthHeight=0x40004,
FrameBufferGetDepth=0x40005,
FrameBufferGetPixelOrder=0x40006,
FrameBufferGetAlphaMode=0x40007,
FrameBufferGetPitch=0x40008,
FrameBufferGetVirtualOffset=0x40009,
FrameBufferGetOverscan=0x4000A,
FrameBufferGetPalette=0x4000B,
FrameBufferGetTouchBuffer=0x4000F,
FrameBufferGetGpioVirtualBuffer=0x40010,
FrameBufferRelease=0x48001,
FrameBufferTestPhysicalWidthHeight=0x44003,
FrameBufferTestVirtualWidthHeight=0x44004,
FrameBufferTestDepth=0x44005,
FrameBufferTestPixelOrder=0x44006,
FrameBufferTestAlphaMode=0x44007,
FrameBufferTestVirtualOffset=0x44009,
FrameBufferTestOverscan=0x4400A,
FrameBufferTestPalette=0x4400B,
FrameBufferTestVSync=0x4400E,
FrameBufferSetPhysicalWidthHeight=0x48003,
FrameBufferSetVirtualWidthHeight=0x48004,
FrameBufferSetDepth=0x48005,
FrameBufferSetPixelOrder=0x48006,
FrameBufferSetAlphaMode=0x48007,
FrameBufferSetVirtualOffset=0x48009,
FrameBufferSetOverscan=0x4800A,
FrameBufferSetPalette=0x4800B,
FrameBufferSetTouchBuffer=0x4801F,
FrameBufferSetGpioVirtualBuffer=0x48020,
FrameBufferSetVSync=0x4800E,
FrameBufferSetBacklight=0x4800F,
GetCommandLine=0x50001,
GetDmaChannels=0x60001,
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