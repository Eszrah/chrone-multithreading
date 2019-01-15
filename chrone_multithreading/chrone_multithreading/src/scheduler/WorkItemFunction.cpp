#include "scheduler/WorkItemFunction.h"

#include <cassert>

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberPoolFunction.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/FiberFunction.h"
#include "scheduler/Fiber.h"
#include "scheduler/Task.h"
#include "scheduler/SyncPrimitive.h"

namespace chrone::multithreading::scheduler
{

void 
WorkItemFunction::MainLoop(
	FiberTaskSchedulerData& scheduler)
{
	std::atomic_bool&	threadsKeepRunning{
		scheduler.threadsData.threadsKeepRunning };

	ThreadFiberData*	threadFibersData{ scheduler.threadFibersData.data() };


	FiberPool&	fiberPool{ scheduler.fiberPool };
	TaskPool&	taskPool{ scheduler.taskPool };

	Semaphore* const	semaphores{ scheduler.semaphores };


	Uint8	threadIndex{ FiberFunction::GetFiberData()->threadIndex };
	ThreadFiberData&	currentThreadFiberData{ threadFibersData[threadIndex] };

	Task	currentTask{};
	Fiber*	dependentFiber{ nullptr };

	while (threadsKeepRunning.load(std::memory_order_acquire))
	{
		if (dependentFiber)
		{
			currentThreadFiberData = FiberFunction::SwitchToFiber(fiberPool, threadFibersData,
				currentThreadFiberData, dependentFiber);
		}

		if (!TaskPoolFunction::TryPopTask(taskPool, currentTask))
		{
			//Wait ???
			assert(false);
			continue;
		}

		const TaskDecl&	decl{ currentTask.decl };
		Semaphore&		dependency{ semaphores[currentTask.dependencyIndex] };

		dependentFiber = nullptr;
		decl.functor(decl.data);

		//memory_order_acq_rel:
		//release: We want to make sure there is no memory reordering between the call and the decrement store
		//acquire: we want to make sure that if the count is 0, then the load will see the dependent fiber
		const Uint	remainingJobCount{ dependency.dependentCounter.fetch_sub(
			1u, std::memory_order_acq_rel) }; //

		if (remainingJobCount == 0)
		{
			//We have synchronized-with the fiber store
			dependentFiber = dependency.dependentFiber.load(
				std::memory_order_relaxed);
			assert( dependentFiber );
		}
	}

	//Check if you tried to quit the loop while there is still a dependent fiber to be executed
	assert(!dependentFiber);
}

}