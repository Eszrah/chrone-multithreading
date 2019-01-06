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

		decl.functor(decl.data);
		assert(false); //=> call the functor in a wrapper to provide argument without memory allocation

		const Uint	remainingJobCount{ dependency.dependentCounter.fetch_sub(
			1u, std::memory_order_release) }; //

		if (remainingJobCount == 0)
		{
			dependentFiber = dependency.dependentFiber.load(
				std::memory_order_relaxed);
		}
	}
}

}