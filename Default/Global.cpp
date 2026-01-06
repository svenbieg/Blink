//============
// Global.cpp
//============

#include "Global.h"


//=======
// Using
//=======

using namespace Concurrency;


//==================
// Con-/Destructors
//==================

Global::~Global()
{
WriteLock lock(s_Mutex);
s_Global=nullptr;
}


//================
// Common Private
//================

Mutex Global::s_Mutex;
Global* Global::s_Global=nullptr;
