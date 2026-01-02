//===============
// ServiceTask.h
//===============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#task-creation

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"


//===========
// Namespace
//===========

namespace Concurrency {


//==============
// Service-Task
//==============

class ServiceTask
{
public:
	// Con-/Destructors
	static inline Handle<Task> Create(VOID (*Procedure)(), Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE)
		{
		auto task=Task::Create(Procedure, Name, StackSize);
		FlagHelper::Set(task->m_Flags, TaskFlags::Locked);
		task->m_LockCount=1;
		return task;
		}
	template <class _owner_t> static inline Handle<Task> Create(_owner_t* Owner, VOID (_owner_t::*Procedure)(), Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE)
		{
		auto task=Task::Create(Owner, Procedure, Name, StackSize);
		FlagHelper::Set(task->m_Flags, TaskFlags::Locked);
		task->m_LockCount=1;
		return task;
		}
	template <class _owner_t> static inline Handle<Task> Create(Handle<_owner_t> const& Owner, VOID (_owner_t::*Procedure)(), Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE)
		{
		auto task=Task::Create(Owner, Procedure, Name, StackSize);
		FlagHelper::Set(task->m_Flags, TaskFlags::Locked);
		task->m_LockCount=1;
		return task;
		}
	template <class _owner_t, class _lambda_t> static inline Handle<Task> Create(_owner_t* Owner, _lambda_t&& Lambda, Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE)
		{
		auto task=Task::Create(Owner, std::forward<_lambda_t>(Lambda), Name, StackSize);
		FlagHelper::Set(task->m_Flags, TaskFlags::Locked);
		task->m_LockCount=1;
		return task;
		}
	template <class _owner_t, class _lambda_t> static inline Handle<Task> Create(Handle<_owner_t> const& Owner, _lambda_t&& Lambda, Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE)
		{
		auto task=Task::Create(Owner, std::forward<_lambda_t>(Lambda), Name, StackSize);
		FlagHelper::Set(task->m_Flags, TaskFlags::Locked);
		task->m_LockCount=1;
		return task;
		}
	template <class _lambda_t> static inline Handle<Task> Create(nullptr_t Owner, _lambda_t&& Lambda, Handle<String> Name="task", SIZE_T StackSize=MemoryHelper::PAGE_SIZE)
		{
		auto task=Task::Create(Owner, std::forward<_lambda_t>(Lambda), Name, StackSize);
		FlagHelper::Set(task->m_Flags, TaskFlags::Locked);
		task->m_LockCount=1;
		return task;
		}
};

}