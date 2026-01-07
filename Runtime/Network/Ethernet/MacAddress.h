//==============
// MacAddress.h
//==============

#pragma once


//=======
// Using
//=======

#include "StringClass.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Ethernet {


//======
// Type
//======

typedef UINT64 MAC_ADDR;
constexpr UINT MAC_ADDR_SIZE=6;


//=============
// MAC-Address
//=============

class MacAddress
{
public:
	// Con-/Destrucotrs
	static MAC_ADDR From(BYTE A0, BYTE A1, BYTE A2, BYTE A3, BYTE A4, BYTE A5);
	static MAC_ADDR FromString(LPCSTR String);

	// Common
	static BOOL IsBroadcast(MAC_ADDR Address);
	static BOOL IsMulticast(MAC_ADDR Address);
	static Handle<String> ToString(MAC_ADDR Address);
};

}}