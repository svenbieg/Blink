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

template <class _owner_t, class... _args_t>
inline Handle<Concurrency::Task> CreateTask(VOID (*Procedure)())
{
using Scheduler=Concurrency::Scheduler;
using Task=Concurrency::Task;
Handle<Task> task=new Concurrency::Details::TaskProcedure(Procedure);
Scheduler::AddTask(task);
return task;
}

template <class _owner_t>
inline Handle<Concurrency::Task> CreateTask(_owner_t* Owner, VOID (_owner_t::*Procedure)())
{
using Scheduler=Concurrency::Scheduler;
using Task=Concurrency::Task;
Handle<Task> task=new Concurrency::Details::TaskMemberProcedure(Owner, Procedure);
Scheduler::AddTask(task);
return task;
}

template <class _owner_t, class _lambda_t>
inline Handle<Concurrency::Task> CreateTask(_owner_t* Owner, _lambda_t&& Lambda)
{
using Scheduler=Concurrency::Scheduler;
using Task=Concurrency::Task;
Handle<Task> task=new Concurrency::Details::TaskLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
Scheduler::AddTask(task);
return task;
}

inline Handle<Concurrency::Task> GetCurrentTask()
{
return Concurrency::Scheduler::GetCurrentTask();
}
