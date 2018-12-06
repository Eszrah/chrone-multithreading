#include "scheduler/TaskPoolFunction.h"

#include "scheduler/TaskPool.h"
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
	TaskPoolFunction::TryGetTask(
		TaskPool& pool, 
		TaskDecl& task)
	{
		LockGuardSpinLock	lock{ pool.taskBuffersLock };
		std::vector<TaskBuffer>&	taskBuffers{ pool.taskBuffers };

		if (taskBuffers.empty()) 
		{
			return false;
		}

		TaskBuffer&	lastBuffer{ taskBuffers.back() };
		std::atomic<Uint>&	bufferCount{ lastBuffer.count };

		const Uint currentCount{ bufferCount.load(std::memory_order_acquire) };

		if (!currentCount)
		{
			return false;
		}




		return true;
	}
}