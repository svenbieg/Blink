//============
// Ethernet.h
//============

#pragma once


//===========
// Namespace
//===========

namespace Network {
	namespace Ethernet {


//==========
// Ethernet
//==========

class Ethernet
{
public:
	// Common
	static VOID Initialize();

private:
	// Common
	static BOOL bInitialized;
};

}}
