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
	static inline VOID Initialize(VOID** Stack, VOID (*TaskProc)(VOID*), VOID* Parameter)
		{
		task_init(Stack, TaskProc, Parameter);
		}
	static inline VOID* RestoreContext(VOID* Stack)
		{
		return task_restore_context(Stack);
		}
	static inline VOID* SaveContext(VOID* Stack)
		{
		return task_save_context(Stack);
		}
	static VOID Switch(UINT Core, Task* Current, Task* Next);
};

}