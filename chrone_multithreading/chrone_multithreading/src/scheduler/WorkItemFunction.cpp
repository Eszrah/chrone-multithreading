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

	std::vector<ThreadFiberData>&	threadFibersData{
		scheduler.threadFibersData };

	FiberPool&	fiberPool{ scheduler.fiberPool };
	TaskPool&	taskPool{ scheduler.taskPool };

	Semaphore* const	semaphores{ scheduler.semaphores };

	Uint8	threadIndex{ FiberFunction::GetFiberData()->threadIndex };
	ThreadFiberData&	threadFiberData{ threadFibersData[threadIndex] };

	Task	currentTask{};
	Fiber*	dependentFiber{ nullptr };

	while (threadsKeepRunning.load(std::memory_order_acquire))
	{
		if (dependentFiber)
		{
			FiberFunction::SwitchToFiber(fiberPool, threadFiberData, dependentFiber);
			threadIndex = FiberFunction::GetFiberData()->threadIndex;
			threadFiberData = threadFibersData[threadIndex];
			assert(threadFiberData.previousFiber);
			FiberPoolFunction::PushFreeFiber(fiberPool, threadFiberData.previousFiber);
			threadFiberData.previousFiber = nullptr;
		}

		if (!TaskPoolFunction::TryPopTask(taskPool, currentTask))
		{
			//Wait ???
			continue;
		}

		const TaskDecl&	decl{ currentTask.decl };

		decl.functor(decl.data);

		Semaphore&	dependency{ semaphores[currentTask.dependencyIndex] };

		auto const	lastDependencyCount{ dependency.dependentCounter.fetch_sub(
			1u, std::memory_order_release) };

		dependentFiber = lastDependencyCount == 1u ? dependency.dependentFiber.load(std::memory_order_relaxed) : nullptr;
	}
}

}