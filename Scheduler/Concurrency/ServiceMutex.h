//================
// ServiceMutex.h
//================

// A mutex raising priority to prevent suspension.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#dynamic-prioritization

#pragma once


//=======
// Using
//=======

#include "Concurrency/Mutex.h"


//===========
// Namespace
//===========

namespace Concurrency {


//================
// Critical-Mutex
//================

class ServiceMutex: public Mutex
{
public:
	// Con-/Destructors
	ServiceMutex()=default;
	ServiceMutex(ServiceMutex const&)=delete;

protected:
	// Common
	BOOL Lock(Task* Current)noexcept override;
	BOOL Lock(Task* Current, AccessMode)noexcept override;
	VOID Unlock(Task* Current)noexcept override;
	VOID Unlock(Task* Current, AccessMode)noexcept override;
};

}