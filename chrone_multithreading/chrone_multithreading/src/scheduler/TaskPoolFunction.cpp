#include "scheduler/TaskPoolFunction.h"

#include "scheduler/TaskPool.h"
#include "scheduler/Task.h"
#include "scheduler/TaskBuffer.h"

#include "std_extension/SpinlockStdExt.h"
#include "AssertMacro.h"

namespace chrone::multithreading::scheduler
{

	bool 
	TaskPoolFunction::Initialize(
		TaskPool& pool,
		Uint32 maxTasksCountPowerOfTwo)
	{
		pool.headIndex = 0u;
		pool.tailIndex = 0u;
		pool.tasksMaxCount = maxTasksCountPowerOfTwo;
		pool.tasks = new Task[maxTasksCountPowerOfTwo];
		return true;
	}


	bool 
	TaskPoolFunction::Clear(
		TaskPool& pool)
	{
		pool.headIndex = 0u;
		pool.tailIndex = 0u;
		pool.tasksMaxCount = 0u;
		delete[] pool.tasks;
		pool.tasks = nullptr;
		return true;
	}


	bool 
	TaskPoolFunction::PushTasks(
		TaskPool& pool, 
		Uint32 count, 
		TaskDecl* tasksDecl, 
		const Uint32 dependencyIndex)
	{
		const Uint32	tasksMaxCountMinusOne{ pool.tasksMaxCount - 1u};
		Task* const		tasks{ pool.tasks };

		Task	tmpTask{};

		for (Uint32 index{ 0u }; index < count; ++index)
		{
			tmpTask = Task{ tasksDecl[index], dependencyIndex };
			LockGuardSpinLock	lock{ pool.taskBuffersLock };
			const Uint32	tailIndex{ pool.tailIndex };

			CHR_ASSERT(tailIndex != pool.headIndex);
			tasks[tailIndex] = std::move(tmpTask);
			pool.tailIndex = (tailIndex + 1u) & tasksMaxCountMinusOne;
		}


		return true;
	}


	bool 
	TaskPoolFunction::TryPopTask(
		TaskPool& pool, 
		Task& task)
	{
		const Uint32	tasksMaxCountMinusOne{ pool.tasksMaxCount - 1u };
		Task* const		tasks{ pool.tasks };
		LockGuardSpinLock	lock{ pool.taskBuffersLock };

		const Uint32	headIndex{ pool.headIndex };
		const Uint32	tailIndex{ pool.tailIndex };

		if (headIndex == tailIndex)
		{
			return false;
		}

		task = std::move(tasks[headIndex]);
		pool.headIndex = (headIndex + 1u) & tasksMaxCountMinusOne;

		return true;
	}

}