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
	Fiber* newFiber,
	Semaphore* syncSemaphore,
	Fiber* syncSrcFiber)
{
	{
	//Setup the new fiber data (to allow it to properly retrieve its thread once it restart)
	FiberData*	fiberData{ fromThreadFiberData.currentFiber->fiberData };
	FiberData*	newFiberData{ newFiber->fiberData };

	//must have been reseted earlier
	assert(!fromThreadFiberData.previousFiber &&
		!fromThreadFiberData.syncSemaphore);
	
	//checking for missing argument
	assert((syncSemaphore && syncSrcFiber) || 
		(!syncSemaphore && !syncSrcFiber));

	assert( (newFiber && (!syncSemaphore && !syncSrcFiber)) ||
	(!newFiber && (syncSemaphore && syncSrcFiber)));

	//We assign the threadData's data
	newFiberData->threadIndex = fiberData->threadIndex;
	fromThreadFiberData.syncSrcFiber = syncSrcFiber;
	fromThreadFiberData.syncSemaphore = syncSemaphore;
	fromThreadFiberData.previousFiber = fromThreadFiberData.currentFiber;
	fromThreadFiberData.currentFiber = newFiber;
	//We switch to the next fiber
	WindowsFiberHelper::SwitchToFiber(newFiber->fiberHandle);
	}

	//From here we are in the new fiber we switched on
	//!!!!!YOU MUST NOT USE THE FUNCTION ARGUMENTS!!!!!

	//We have to retrieved the good thread data through the stored thread index (thread local variables can't be used)
	const Uint8	toThreadIndex{ FiberFunction::GetFiberData()->threadIndex };
	ThreadFiberData& toThreadFiberData{ threadsFiberData[toThreadIndex] };
	Semaphore*	toSyncSemaphore{ toThreadFiberData.syncSemaphore };

	//Checking we either come from a fiber we wanted to switch on for sync or because we retrieved a waiting fiber
	assert(toSyncSemaphore || fromThreadFiberData.previousFiber);


	if (toThreadFiberData.previousFiber)
	{
		//it is not valid to have a sync semaphore
		assert(!toSyncSemaphore);
		FiberPoolFunction::PushFreeFiber(fiberPool, toThreadFiberData.previousFiber);
		toThreadFiberData.previousFiber = nullptr;
	}
	else if (toSyncSemaphore)
	{
		//it is not valid to have a previous fiber
		assert(!fromThreadFiberData.previousFiber);
		std::atomic<Fiber*>&	dependentFiberAtomic{ 
			toSyncSemaphore->dependentFiber };

		//We want to make sure the dependent fiber stored in the fiber we come from is stored before we decrement and store
		dependentFiberAtomic.store(toThreadFiberData.syncSrcFiber, std::memory_order_relaxed);
		//We make sure the dec store is not reordered with the previous store -> write release operaion
		const Uint	remainingJobCount{ 
			toSyncSemaphore->dependentCounter.fetch_sub(
				1, std::memory_order_release) };

		//We check if we are the last one who dec the counter
		if (remainingJobCount == 0)
		{
			//we are the thread who pushed the fiber value so we don't have to fetch it
			//make sure the store can't be reordered before the previous store -> write release
			dependentFiberAtomic.store(nullptr, std::memory_order_release );
			toThreadFiberData.syncSrcFiber = nullptr;
			toThreadFiberData.syncSemaphore = nullptr;

			//make sure to write to memory before switching to the fiber
			toThreadFiberData = SwitchToFiber(fiberPool, threadsFiberData, 
				toThreadFiberData, toThreadFiberData.syncSrcFiber );
		}
	}
	
	return toThreadFiberData;
}


}