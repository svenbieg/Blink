//=================
// SouthBridge.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "PcieHost.h"
#include "SouthBridge.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Pcie {


//==========
// Settings
//==========

constexpr PCIE_DEVICE_INFO PCIE_DEVICE_RP1={ 0x20000, 0, 0 };


//========
// Common
//========

Handle<SouthBridge> SouthBridge::Open()
{
if(s_Default)
	return s_Default;
auto pcie=PcieHost::Open();
if(!pcie)
	return nullptr;
if(!pcie->EnableDevice(PCIE_DEVICE_RP1))
	return nullptr;
s_Default=new SouthBridge();
return s_Default;
}


//==========================
// Con-/Destructors Private
//==========================

SouthBridge::SouthBridge()
{}


//================
// Common Private
//================

Handle<SouthBridge> SouthBridge::s_Default;

}}
