#include "scheduler/TaskFunction.h"

#include <atomic>
#include <mutex>

#include "scheduler/Task.h"
#include "scheduler/Semaphore.h"

namespace chrone::multithreading::scheduler
{

Fiber* 
TaskFunction::ExecuteTask(
	Task& task)
{
	const TaskDecl&	decl{ task.decl };

	decl.functor(decl.data);

	//RETRIEVE THE DEPENDENCY WITH THE INDEX
	Semaphore	dependency{};

	auto const	lastDependencyCount{ dependency.dependentCounter.fetch_sub(
		1u, std::memory_order_release) };

	if (1u == lastDependencyCount)
	{		 
		return dependency.dependentFiber.load(std::memory_order_relaxed);
	}

	return nullptr;
}

}

/*
	auto const	lastDependencyCount{ dependency.dependentCounter->fetch_sub(
		1u, std::memory_order_release) };

	if (1u == lastDependencyCount)
	{
		if (dependency.fence)
		{
			dependency.fence->notify_one();
		}

		return dependency.dependentFiber->load(std::memory_order_relaxed);
	}
*/