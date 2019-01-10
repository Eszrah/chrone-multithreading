#include "scheduler/WorkerFiberEntryPoint.h"

#include <cassert>

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberPoolFunction.h"
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
	const Uint8	threadIndex{ fiberData->threadIndex };
	FiberTaskSchedulerData*	scheduler{ fiberData->scheduler };
	ThreadFiberData&	threadFiberData{
		scheduler->threadFibersData[threadIndex] };

	//you may come from a mainLoop switchToFiber or from a shutdown switchToFiber

	//threadFiberData.previousFiber is nullptr if you come from a thread which never switched to another fiber
	//OR if you come from a thread's Shutdown function
	if (threadFiberData.previousFiber)
	{
		FiberPoolFunction::PushFreeFiber(scheduler->fiberPool, threadFiberData.previousFiber);
		threadFiberData.previousFiber = nullptr;
	}

	if (scheduler->threadsData.threadsKeepRunning)
	{
		WorkItemFunction::MainLoop(*fiberData->scheduler);
	}

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
	ThreadFiberData*	threadFibersData{ scheduler->threadFibersData.data() };
	ThreadFiberData&	threadFiberData{ threadFibersData[threadIndex] };
	Fiber&	threadFiber{ scheduler->threadsFibers[threadIndex] };
	const bool	threadsShutdownState{ threadsData.threadsShutdownState[threadIndex] };

	assert(!threadFiberData.previousFiber);

	CHECK IF THIS IS A VALID ASSUMPTION  that if !threadsShutdownState then you have a non nulll Fiber!!!!
	To me you can't have ' a non NULL previous fiber
	if (!threadsShutdownState)
	{
		//Pushing the old fiber if there is one, could contains another native thread fiber or a classic fiber
		assert(threadFiberData.previousFiber);
		FiberPoolFunction::PushFreeFiber(fiberPool, threadFiberData.previousFiber);
		threadFiberData.previousFiber = nullptr;

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
	FiberFunction::SwitchToFiber(fiberPool, threadFibersData, threadFiberData, &threadFiber);
}

}