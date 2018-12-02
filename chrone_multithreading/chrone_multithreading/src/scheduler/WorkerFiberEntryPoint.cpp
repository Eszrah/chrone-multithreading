#include "scheduler/WorkerFiberEntryPoint.h"

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberData.h"
#include "scheduler/FiberFunction.h"
#include "scheduler/WorkItemFunction.h"

namespace chrone::multithreading::scheduler
{

void __stdcall
WorkerFiberEntryPoint(
	void* data)
{
	FiberData*	fiberData{ static_cast<FiberData*>(data) };

	WorkItemFunction::MainLoop(*fiberData->scheduler);
	FiberEntryPointFunction::_Shutdown();
}


void 
FiberEntryPointFunction::_Shutdown()
{
	//SHOULD NOT BE EXECUTED TWICE !!!!!
	const FiberData*	fiberData{ FiberFunction::GetFiberData() };
	const Uint8	threadIndex{ fiberData->threadIndex };
	FiberTaskSchedulerData*	scheduler{ fiberData->scheduler };
	ThreadsData&	threadsData{ scheduler->threadsData };
	FiberPool&	fiberPool{ scheduler->fiberPool };
	ThreadFiberData&	threadFibersData{ scheduler->threadFibersData[threadIndex] };
	Fiber&	threadFiber{ scheduler->threadsFibers[threadIndex] };
	const bool	threadsShutdownState{ threadsData.threadsShutdownState[threadIndex] };

	if (!threadsShutdownState)
	{
		//Pushing the old fiber if there is one, could contains another native thread fiber or a classic fiber
		FiberFunction::PushPreviousFiber(fiberPool, threadFibersData);
		threadsData.threadsShutdownState[threadIndex] = true;
		//Making sure everybody have free its old fiber
		threadsData.threadsCountSignal.fetch_add(1, std::memory_order_release);
		while (threadsData.threadsBarrier.load(std::memory_order_acquire));
		//signaling we have passed the barrier
		threadsData.threadsCountSignal.fetch_add(1, std::memory_order_release);
	}

	//Waiting for all other threads to complete their operations
	threadsData.threadsCountSignal.fetch_add(1, std::memory_order_release);
	while (threadsData.threadsCountSignal.load(std::memory_order_acquire) != 0u);
	//Switching to the thread original fiber
	FiberFunction::SwitchToFiber(fiberPool, threadFibersData, &threadFiber);
}

}