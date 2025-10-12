//==============
// TaskHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include <task.h>


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
	static inline VOID Initialize(SIZE_T* StackPointer, VOID (*TaskProc)(VOID*), VOID* Parameter)
		{
		task_init(StackPointer, TaskProc, Parameter);
		}
	static inline SIZE_T RestoreContext(SIZE_T StackPointer)
		{
		return task_restore_context(StackPointer);
		}
	static inline SIZE_T SaveContext(SIZE_T StackPointer)
		{
		return task_save_context(StackPointer);
		}
	static VOID Switch(UINT Core, Task* Current, Task* Next);
};

}