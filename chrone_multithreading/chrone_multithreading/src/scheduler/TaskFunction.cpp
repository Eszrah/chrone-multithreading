#include "scheduler/TaskFunction.h"

#include <atomic>
#include <mutex>

#include "scheduler/Task.h"

namespace chrone::multithreading::scheduler
{

Fiber* 
TaskFunction::ExecuteTask(
	Task& task)
{
	const TaskDecl&	decl{ task.decl };
	TaskDependency&	dependency{ task.dependency };

	decl.functor(decl.data);

	if (dependency.dependentCounter)
	{
		auto const	lastDependencyCount{ dependency.dependentCounter->fetch_sub(
			1u, std::memory_order_release) };

		if (1u == lastDependencyCount)
		{
			if (dependency.fence)
			{
				dependency.fence->notify_one();
			}

			return dependency.dependentFiber;
		}
	}

	return nullptr;
}

}