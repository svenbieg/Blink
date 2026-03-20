//==============
// TaskHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class Task;


//=============
// Task-Helper
//=============

class TaskHelper
{
public:
	// Common
	static VOID Initialize(SIZE_T* StackPointer, VOID (*TaskProc)(VOID*), VOID* Parameter)noexcept;
	static SIZE_T RestoreContext(SIZE_T StackPointer)noexcept;
	static SIZE_T SaveContext(SIZE_T StackPointer)noexcept;
	static VOID Switch(UINT Core, Task* Current, Task* Next)noexcept;
};

}