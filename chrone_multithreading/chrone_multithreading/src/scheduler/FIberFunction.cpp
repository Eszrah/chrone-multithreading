#include "scheduler/FiberFunction.h"

#include <atomic>
#include <cassert>

#include "scheduler/FiberPoolFunction.h"

#include "scheduler/WindowsFiberHelper.h"
#include "scheduler/ThreadFiberData.h"
#include "scheduler/FiberData.h"
#include "scheduler/SyncPrimitive.h"
#include "scheduler/Fiber.h"

namespace chrone::multithreading::scheduler
{

const FiberData* 
FiberFunction::GetFiberData()
{
	return WindowsFiberHelper::GetCurrentFiberData<FiberData>();
}


ThreadFiberData&
FiberFunction::SwitchToFiber(
	FiberPool& fiberPool,
	ThreadFiberData* threadsFiberData, 
	ThreadFiberData& fromThreadFiberData,
	Fiber* newFiber)
{
	{
	//Setup the new fiber data (to allow it to properly retrieve its thread once it restart)
	FiberData*	fiberData{ fromThreadFiberData.currentFiber->fiberData };
	FiberData*	newFiberData{ newFiber->fiberData };

	assert(!fromThreadFiberData.previousFiber && 
		!fromThreadFiberData.syncSemaphore);
	
	newFiberData->threadIndex = fiberData->threadIndex;
	fromThreadFiberData.previousFiber = fromThreadFiberData.currentFiber;
	fromThreadFiberData.currentFiber = newFiber;
	WindowsFiberHelper::SwitchToFiber(newFiber->fiberHandle);
	}

	const Uint8	toThreadIndex{ FiberFunction::GetFiberData()->threadIndex };
	ThreadFiberData& toThreadFiberData{ threadsFiberData[toThreadIndex] };
	Semaphore*	syncSemaphore{ toThreadFiberData.syncSemaphore };

	assert(syncSemaphore || fromThreadFiberData.previousFiber);

	if (toThreadFiberData.previousFiber)
	{
		assert(!syncSemaphore);
		FiberPoolFunction::PushFreeFiber(fiberPool, toThreadFiberData.previousFiber);
		toThreadFiberData.previousFiber = nullptr;
	}
	else if (syncSemaphore)
	{
		assert(!fromThreadFiberData.previousFiber);
		std::atomic<Fiber*>&	dependentFiberAtomic{ 
			syncSemaphore->dependentFiber };

		//We want to make sure the dependant fiber stored in the fiber we come from is stored before we decrement and store
		const Uint	remainingJobCount{ 
			syncSemaphore->dependentCounter.fetch_sub(
				1, std::memory_order_release) };

		if (remainingJobCount == 0)
		{
			Fiber*	dependentFiber{ 
				dependentFiberAtomic.load(std::memory_order_relaxed) };

			dependentFiberAtomic.store(nullptr, std::memory_order_relaxed);

			//make sure to write to memory before switching to the fiber
			toThreadFiberData = SwitchToFiber(fiberPool, threadsFiberData, 
				toThreadFiberData, dependentFiber);
		}
	}
	
	return toThreadFiberData;
}


}