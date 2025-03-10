//==============
// TaskHelper.h
//==============

#pragma once


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
	static VOID Initialize(VOID** Stack, VOID (*TaskProc)(VOID*), VOID* Parameter);
	static SIZE_T RestoreContext(VOID* Stack);
	static VOID* SaveContext(VOID* Stack);
	static VOID Switch(UINT Core, Task* Current, Task* Next);
};

}