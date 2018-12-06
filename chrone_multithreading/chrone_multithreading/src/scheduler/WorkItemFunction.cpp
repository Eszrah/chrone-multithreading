#include "scheduler/WorkItemFunction.h"

#include <cassert>

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberPoolFunction.h"
#include "scheduler/FiberFunction.h"
#include "scheduler/Fiber.h"

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

	Uint8	threadIndex{ FiberFunction::GetFiberData()->threadIndex };
	ThreadFiberData&	threadFiberData{ threadFibersData[threadIndex] };

	while (threadsKeepRunning.load(std::memory_order_acquire))
	{
		//Try to find a task to execute


		Task&	currentTask{ threadFiberData.currentFiber->task };



		bool const	isTerminalFiber{ 
			FiberFunction::ExecuteFiberTask(currentTask) };
		
		Fiber*	newFiber{ isTerminalFiber ?
			currentTask.dependencyFiber->load(std::memory_order_relaxed) 
			: nullptr };
		
		currentTask = Task{};
		
		if (threadsKeepRunning.load(std::memory_order_acquire))
		{
			//WARNING: OLD CODE WAS : if (!newFiber && !_fiberPool.TryPopWaitingFiber(newFiber))
			if (!newFiber)
			{
				FiberPoolFunction::TryPopReadyFiber(fiberPool, newFiber);
			}
			
			if (newFiber)
			{
				FiberFunction::SwitchToFiber(fiberPool, threadFiberData, newFiber);
				threadIndex = FiberFunction::GetFiberData()->threadIndex;
				threadFiberData = threadFibersData[threadIndex];
				assert(threadFiberData.previousFiber);
				FiberPoolFunction::PushFreeFiber(fiberPool, threadFiberData.previousFiber);
				threadFiberData.previousFiber = nullptr;
			}
		}
	}
}

}