#include "scheduler/WorkerThreadEntryPoint.h"

#include <vector>
#include <thread>
#include <algorithm>
#include <cassert>

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberPoolFunction.h"
#include "scheduler/FiberFunction.h"
#include "scheduler/WindowsFiberHelper.h"
#include "scheduler/WorkItemFunction.h"

#include "std_extension/SpinlockStdExt.h"

namespace chrone::multithreading::scheduler
{

void 
WorkerThreadFunction::EntryPoint(
	WorkerThreadFuncData funcData)
{
	if (!_Initialize(funcData)) { return; }

	WorkItemFunction::MainLoop(*funcData.scheduler);

	_Shutdown();
}

bool 
WorkerThreadFunction::_Initialize(
	WorkerThreadFuncData funcData)
{
	Uint							threadIndex{ funcData.threadIndex };
	FiberTaskSchedulerData*			scheduler{ funcData.scheduler };
	ThreadsData&					threadsData{ scheduler->threadsData };
	std::vector<FiberData>&			fibersData{ scheduler->fibersData };
	std::vector<ThreadFiberData>&	threadFibersData{ scheduler->threadFibersData };
	std::vector<Fiber>&	threadsFibers{ scheduler->threadsFibers };


	FiberData&	fiberData{ fibersData[threadIndex] };
	fiberData.threadIndex = threadIndex;

	void*	threadFiberHandle{
		WindowsFiberHelper::ConvertThreadToFiber(&fiberData) };
	if (!threadFiberHandle)
	{
		threadsData.threadsEmitError.store(true, std::memory_order_relaxed);
		threadsData.threadsCountSignal.fetch_add(1, std::memory_order_release);
		return false;
	}


	Fiber&	threadFiber{ threadsFibers[threadIndex] };
	threadFiber = Fiber{ threadFiberHandle, &fiberData };

	ThreadFiberData&	threadFiberData{ threadFibersData[threadIndex] };
	threadFiberData.previousFiber = nullptr;
	threadFiberData.currentFiber = &threadFiber;

	//signal everything is fine and wait
	threadsData.threadsCountSignal.fetch_add(1, std::memory_order_release);
	while (threadsData.threadsBarrier.load(std::memory_order_acquire));
	//signaling we have passed the barrier
	threadsData.threadsCountSignal.fetch_add(1, std::memory_order_release);
	return !threadsData.threadsEmitError.load(std::memory_order_acquire);
}

bool 
WorkerThreadFunction::_Shutdown()
{
	FiberData*	fiberData{ FiberFunction::GetFiberData() };

	FiberTaskSchedulerData*	scheduler{ fiberData->scheduler };
	FiberPool&	fiberPool{ scheduler->fiberPool };
	ThreadsData&	threadsData{ scheduler->threadsData };

	Uint	threadIndex{ fiberData->threadIndex };
	std::vector<ThreadFiberData>&	threadFibersData{ scheduler->threadFibersData };
	ThreadFiberData&	threadFiberData{ threadFibersData[threadIndex] };

	if (threadsData.threadsShutdownState[threadIndex])
	{
		return true;
	}


	//Pushing the old fiber if there is one(could go to shutdown without never having switched to another fiber)
	//could contains another native thread fiber or a classic fiber
	if (threadFiberData.previousFiber)
	{
		FiberPoolFunction::PushFreeFiber(fiberPool, threadFiberData.previousFiber);
		threadFiberData.previousFiber = nullptr;
	}
	
	//Making sure everybody have free its old fiber
	threadsData.threadsCountSignal.fetch_add(1, std::memory_order_release);
	while (threadsData.threadsBarrier.load(std::memory_order_acquire));
	
	//signaling we have passed the barrier
	threadsData.threadsCountSignal.fetch_add(1, std::memory_order_release);

	//Getting and Switching to a native fiber
	Fiber*	nativeFiber{ _GetFreeNativeFiber(fiberPool, scheduler->threadsFibers) };
	threadsData.threadsShutdownState[threadIndex] = true;
	FiberFunction::SwitchToFiber(fiberPool, threadFiberData, nativeFiber);

	//You are now in another fiber
	threadIndex = FiberFunction::GetFiberData()->threadIndex;
	threadFiberData = threadFibersData[threadIndex];
	assert(threadFiberData.previousFiber);
	FiberPoolFunction::PushFreeFiber(fiberPool, threadFiberData.previousFiber);
	threadFiberData.previousFiber = nullptr;
	return true;
}

Fiber* 
WorkerThreadFunction::_GetFreeNativeFiber(
	FiberPool& fiberPool, 
	const std::vector<Fiber>& threadFibers)
{
	Fiber*	foundFIber{ nullptr };
	LockGuardSpinLock	lock{ fiberPool.freeFibersLock };
	std::vector<Fiber*>&	freeFibers{ fiberPool.freeFibers };

	auto findIt{ std::find_if(freeFibers.begin(), freeFibers.end(),
		[threadFibers = std::cref(threadFibers)](const Fiber* fiberCandidate)
	{
		const std::vector<Fiber>& filteredCandidate = threadFibers.get();

		return std::none_of(filteredCandidate.cbegin(), filteredCandidate.cend(),
			[fiberCandidate](const Fiber& threadFiber)
		{ return threadFiber.fiberHandle == fiberCandidate->fiberHandle;
		});
	}) };

	if (findIt != freeFibers.end())
	{
		foundFIber = *findIt;
		freeFibers.erase(findIt);
	}

	return foundFIber;
}

}
