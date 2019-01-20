#include "scheduler/WorkerFiberEntryPoint.h"

#include <cassert>

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberPoolFunction.h"
#include "scheduler/FiberData.h"
#include "scheduler/FiberFunction.h"
#include "scheduler/FiberMainLoop.h"

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
		FiberMainLoop::MainLoop(*fiberData->scheduler);
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

	if (!threadsShutdownState)
	{
		threadsData.threadsShutdownState[threadIndex] = true;
		//Making sure everybody have free its old fiber
		threadsData.threadsCountSignal.fetch_add(1, std::memory_order_seq_cst);
		while (threadsData.threadsBarrier.load(std::memory_order_seq_cst));
		//signaling we have passed the barrier
		threadsData.threadsCountSignal.fetch_add(1, std::memory_order_seq_cst);
	}

	//Waiting for all other threads to complete their operations
	threadsData.threadsCountSignal.fetch_add(1, std::memory_order_seq_cst);
	while (threadsData.threadsCountSignal.load(std::memory_order_seq_cst) != 0u);
	//Switching to the thread original fiber
	FiberFunction::SwitchToFiber(fiberPool, threadFibersData, threadFiberData, &threadFiber);
}

}