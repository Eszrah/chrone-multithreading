#include "scheduler/TaskPoolFunction.h"

#include "scheduler/TaskPool.h"
#include "scheduler/Task.h"
#include "std_extension/SpinlockStdExt.h"

namespace chrone::multithreading::scheduler
{
	bool 
	TaskPoolFunction::Initialize(
			TaskPool& pool)
	{
		pool.tasks.begin = nullptr;
		pool.tasks.end = nullptr;
		return true;
	}


	bool 
	TaskPoolFunction::Shutdown(
		TaskPool& pool)
	{
		pool.tasks.begin = nullptr;
		pool.tasks.end = nullptr;
		return true;
	}

	
	bool 
	TaskPoolFunction::PushTasks(
		TaskPool& pool,
		Uint32 count,
		TaskNodeList pushedTaskList,
		TaskDependency dependency)
	{
		Spinlock&	spinLock{ pool.taskBuffersLock };
		TaskNodeList&	tasks{ pool.tasks };

		pushedTaskList.end->next = nullptr;

		if (tasks.end)
		{
			tasks.end->next = pushedTaskList.begin;
		}
		else
		{
			tasks.begin = pushedTaskList.begin;
			tasks.end = pushedTaskList.begin;
		}
		
		return true;
	}

	bool
	TaskPoolFunction::TryPopTask(
		TaskPool& pool, 
		Task& task)
	{
		LockGuardSpinLock	lock{ pool.taskBuffersLock };
		TaskNodeList&	tasks{ pool.tasks };
		TaskNode*	tasksBegin{ tasks.begin };
		TaskNode*	tasksEnd{ tasks.end };

		if (!tasksBegin)
		{
			return false;
		}

		task = std::move(tasksBegin->task);
		tasksBegin = tasksBegin->next;
		
		if (!tasksBegin)
		{
			tasksEnd = nullptr;
		}

		tasks.begin = tasksBegin;
		tasks.end = tasksEnd;

		return true;
	}
}