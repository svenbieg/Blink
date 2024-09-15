//==============
// Ethernet.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Ethernet.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Ethernet {


//==========
// Settings
//==========

constexpr UINT ETH_ADDR_LEN=6;


//========
// Common
//========

VOID Ethernet::Initialize()
{
if(bInitialized)
	return;
bInitialized=true;
}


//================
// Common Private
//================

BOOL Ethernet::bInitialized=false;

}}
