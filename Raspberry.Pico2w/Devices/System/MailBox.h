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
// Mail-Box
//==========

class MailBox
{
public:
	// Common
	static VOID Clear();
	static UINT Read();
	static BOOL TryRead(UINT* Value);
	static VOID Write(UINT Value);
};

}}