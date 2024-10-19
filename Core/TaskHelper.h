//==============
// TaskHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Concurrency/ScopedLock.h"
#include "Concurrency/SharedLock.h"
#include "Concurrency/Signal.h"
#include "Concurrency/SpinLock.h"
#include "Concurrency/TaskLock.h"


//========
// Common
//========

template <class... _args_t>
inline Handle<Concurrency::Task> CreateTask(VOID (*Procedure)(_args_t...), _args_t... Arguments)
{
using Scheduler=Concurrency::Scheduler;
using Task=Concurrency::Task;
Handle<Task> task=new Concurrency::Details::TaskTyped<_args_t...>(Procedure, Arguments...);
Scheduler::AddTask(task);
return task;
}

template <class _owner_t, class... _args_t>
inline Handle<Concurrency::Task> CreateTask(_owner_t* Owner, VOID (_owner_t::*Procedure)(_args_t...), _args_t... Arguments)
{
using Scheduler=Concurrency::Scheduler;
using Task=Concurrency::Task;
Handle<Task> task=new Concurrency::Details::TaskOwned<_owner_t, _args_t...>(Owner, Procedure, Arguments...);
Scheduler::AddTask(task);
return task;
}

inline Handle<Concurrency::Task> GetCurrentTask()
{
return Concurrency::Scheduler::GetCurrentTask();
}
