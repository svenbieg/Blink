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
	static VOID Switch(UINT Core, Task* Current, Task* Next)noexcept;
};

}