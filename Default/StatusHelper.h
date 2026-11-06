//================
// StatusHelper.h
//================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#pragma once


//=======
// Using
//=======

#include "Exception.h"
#include "FlagHelper.h"
#include "Status.h"


//===============
// Status-Helper
//===============

class StatusHelper
{
public:
	// Common
	static inline BOOL Failed(Status Status)noexcept
		{
		return FlagHelper::Get(Status, Status::Error);
		}
	static inline BOOL Succeeded(Status Status)noexcept
		{
		return !FlagHelper::Get(Status, Status::Error);
		}
	static VOID ThrowIfFailed(Status Status);
};
