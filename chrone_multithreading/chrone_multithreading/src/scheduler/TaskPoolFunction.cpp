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
		return false;
	}


	bool 
	TaskPoolFunction::Shutdown(
		TaskPool& pool)
	{
		return false;
	}



	
	bool 
	TaskPoolFunction::PushTasks(
		TaskPool& pool,
		Uint32 count,
		const TaskDecl* tasksDecl, 
		TaskDependency dependency)
	{
		Spinlock&	spinLock{ pool.taskBuffersLock };
		std::vector<Task>&	taskBuffer{ pool.taskBuffer };

		for (Uint32 index{ 0u }; index < count; ++index)
		{
			LockGuardSpinLock	lock{ pool.taskBuffersLock };
			taskBuffer.emplace_back(tasksDecl[index], dependency);
		}

		return true;
	}

	bool
	TaskPoolFunction::TryGetTask(
		TaskPool& pool, 
		Task& task)
	{
		LockGuardSpinLock	lock{ pool.taskBuffersLock };
		std::vector<Task>&	taskBuffer{ pool.taskBuffer };

		if (taskBuffer.empty())
		{
			return false;
		}

		task = std::move(taskBuffer.back());

		taskBuffer.pop_back();

		return true;
	}
}