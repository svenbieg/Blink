//============
// PcieHost.h
//============

#pragma once


//===========
// Namespace
//===========

namespace Devices {
	namespace Pcie {


//========
// Device
//========

typedef struct
{
UINT Id;
BYTE Slot;
BYTE Function;
}PCIE_DEVICE_INFO;


//===========
// Pcie-Host
//===========

class PcieHost: public Object
{
public:
	// Common
	BOOL EnableDevice(PCIE_DEVICE_INFO const& Device);
	static Handle<PcieHost> Open();

private:
	// Con-/Destructors
	PcieHost();

	// Common
	UINT EncodeBarSize(UINT64 Size);
	UINT GetBitCount(UINT64 Value);
	VOID Initialize();
	BOOL IsConnected();
	SIZE_T MapConfiguration(UINT Bus, UINT Slot, UINT Function);
	UINT MdioAddress(UINT Port, UINT Register, UINT Command);
	VOID MdioRead(UINT Port, BYTE Register, UINT* Value);
	VOID MdioWrite(UINT Port, BYTE Register, WORD Value);
	VOID Reset();
	VOID SetOutboundWindow(UINT Id, SIZE_T CpuAddress, SIZE_T PcieAddress, SIZE_T Size);
	BOOL m_Initialized;
	static Handle<PcieHost> s_Default;
};

}}
