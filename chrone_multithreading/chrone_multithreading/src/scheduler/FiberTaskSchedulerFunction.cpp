#include "scheduler/FiberTaskSchedulerFunction.h"

#include <algorithm>
#include <numeric>
#include <stdlib.h>

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberPoolFunction.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/WindowsFiberHelper.h"
#include "scheduler/WorkerThreadEntryPoint.h"
#include "scheduler/WorkerFiberEntryPoint.h"
#include "scheduler/Fence.h"

#include "std_extension/SpinlockStdExt.h"

namespace chrone::multithreading::scheduler
{

bool
FiberTaskSchedulerFunction::Initialize(
	FiberTaskSchedulerData& scheduler,
	const Uint32 threadCount, 
	const Uint32 fiberCount,
	const Uint32 fenceMaxCount)
{
	if (!threadCount ||
		!fiberCount || fiberCount < threadCount)
	{
		return false;
	}

	ThreadsData&	threadsData{ scheduler.threadsData };
	std::vector<ThreadFiberData>&	threadFibersData{ scheduler.threadFibersData };

	std::vector<Fiber>&	threadsFibers{ scheduler.threadsFibers };
	std::vector<Fiber>&	fibers{ scheduler.fibers };
	std::vector<FiberData>&	fibersData{ scheduler.fibersData };
	std::vector<std::thread*>&	threads{ threadsData.threads };
	FiberPool&	fiberPool{ scheduler.fiberPool };

	scheduler.fenceMaxCount = 0u;

	threadsData.threadsKeepRunning = true;
	threadsData.threadsBarrier = true;
	threadsData.threadsEmitError = false;
	threadsData.threadsCountSignal = 0u;
	threadsData.threadsShutdownState.resize(threadCount, false);

	scheduler.fenceMaxCount = fenceMaxCount;
	scheduler.fences.resize(fenceMaxCount);
	scheduler.freeFencesIndices.resize(fenceMaxCount);
	std::iota(scheduler.freeFencesIndices.begin(), 
		scheduler.freeFencesIndices.end(), 0);


	threads.resize(threadCount);
	threadsFibers.resize(threadCount);
	threadFibersData.resize(threadCount);
	fibersData.reserve(threadCount + fiberCount);
	fibers.reserve(fiberCount);
	
	FiberPoolFunction::Reserve(fiberPool, fiberCount);

	for (Uint index{ 0u }; index < threadCount + fiberCount; ++index)
	{
		fibersData.emplace_back(0xFFFFFFFF, &scheduler);
	}

	for (Uint index{ 0u }, fiberDataIndex{ threadCount }; index < fiberCount; ++index, ++fiberDataIndex)
	{
		FiberData*	fiberData{ &fibersData[fiberDataIndex] };
		fibers.emplace_back(WindowsFiberHelper::AllocateHFiber(0u,
			WorkerFiberEntryPoint, fiberData), fiberData);
	}

	FiberPoolFunction::PushFreeFibers_NotConcurrent(fiberPool, fiberCount, fibers.data());

	//Checking all fibers have been well created
	bool const	checkFibersValid{ std::all_of(fibers.cbegin(), fibers.cend(),
		[](Fiber const& fiber) { return fiber.fiberHandle != nullptr; }) };

	if (!checkFibersValid)
	{
		FiberPoolFunction::Clear(fiberPool);
		return false;
	}

	//Creating threads
	for (auto index{ 0u }; index < threadCount; ++index)
	{
		threads[index] = new std::thread(WorkerThreadFunction::EntryPoint,
			WorkerThreadFuncData{ index, &scheduler });
	}

	//Waiting all threads have finished initialization
	_WaitAnddResetCounter(threadsData.threadsCountSignal, threadCount);
	threadsData.threadsKeepRunning = threadsData.threadsEmitError ? false : true;
	threadsData.threadsBarrier.store(false, std::memory_order_release);

	if (threadsData.threadsEmitError)
	{
		_JoinThreads(scheduler.threadsData);
		FiberPoolFunction::Clear(fiberPool);
		return false;
	}

	//making sure they all have passed the barrier
	_WaitAnddResetCounter(threadsData.threadsCountSignal, threadCount);

	return true;
}


bool
FiberTaskSchedulerFunction::Shutdown(
	FiberTaskSchedulerData& scheduler)
{
	ThreadsData&	threadsData{ scheduler.threadsData };
	const auto	threadCount{ threadsData.threads.size() };

	//Signaling to all threads it is time to exit
	threadsData.threadsBarrier.store(true, std::memory_order_release);
	threadsData.threadsCountSignal.store(0u, std::memory_order_release);
	threadsData.threadsKeepRunning.store(false, std::memory_order_release);
	//Wait all threads have pushed their old fiber and reached the _threadsBarrier
	_WaitAnddResetCounter(threadsData.threadsCountSignal, threadCount);
	threadsData.threadsBarrier.store(false, std::memory_order_release);
	auto const threadCountTwice{ threadCount * 2u };
	while (threadsData.threadsCountSignal != threadCountTwice);
	threadsData.threadsCountSignal.store(0u, std::memory_order_release);

	_JoinThreads(threadsData);
	_Clear(scheduler);

	return true;
}


HFence 
FiberTaskSchedulerFunction::AllocateFence(
	FiberTaskSchedulerData& scheduler)
{
	return _AllocateFence(scheduler);
}


void 
FiberTaskSchedulerFunction::FreeFence(
	FiberTaskSchedulerData& scheduler, 
	HFence& fence)
{
	_FreeFence(scheduler, static_cast<Fence*>(fence));
}


bool
FiberTaskSchedulerFunction::PushTasks(
	FiberTaskSchedulerData& scheduler,
	Uint32 count, 
	const TaskDecl* tasksDecl, 
	HFence hFence)
{
	TaskDependency	dependency{};
	Fence*	fence{ static_cast<Fence*>(hFence) };

	if (fence)
	{
		dependency.dependentCounter = &fence->dependantCounter;
		dependency.fence = &fence->conditionVariable;
		dependency.dependentFiber = nullptr;
	}
	
	TaskPoolFunction::PushTasks(
		scheduler.taskPool, count, tasksDecl, dependency);

	hFence = fence;
	return true;
}


bool 
FiberTaskSchedulerFunction::WaitFence(
	FiberTaskSchedulerData& scheduler,
	HFence& hFence)
{
	Fence*	fence{ static_cast<Fence*>(hFence) };
	std::unique_lock	lock{ fence->mutex };

	fence->conditionVariable.wait(lock);

	return true;
}


Fence*
FiberTaskSchedulerFunction::_AllocateFence(
	FiberTaskSchedulerData& scheduler)
{
	LockGuardSpinLock	lock{ scheduler.fenceLock };
	std::vector<Uint32>&	freeFencesIndices{ scheduler.freeFencesIndices };
	std::vector<Fence>&		fences{ scheduler.fences };

	Uint32	fenceIndex{ 0xFFFFFFFF };
	const Uint32	freeFenceIndiceCount{ freeFencesIndices.size() };

	fenceIndex = freeFencesIndices.back();
	freeFencesIndices.pop_back();

	return &fences[fenceIndex];
}


void 
FiberTaskSchedulerFunction::_FreeFence(
	FiberTaskSchedulerData& scheduler, 
	Fence* fence)
{
	LockGuardSpinLock	lock{ scheduler.fenceLock };
	std::vector<Uint32>&	freeFencesIndices{ scheduler.freeFencesIndices };
	std::vector<Fence>&		fences{ scheduler.fences };

	Uint32	fenceIndex{ fence - fences.data() };
	freeFencesIndices.push_back(fenceIndex);
}


void
FiberTaskSchedulerFunction::_WaitAnddResetCounter(
	std::atomic<Uint>& counter, 
	Uint count)
{
	while (counter != count);
	counter = 0u;
}


void 
FiberTaskSchedulerFunction::_JoinThreads(
	ThreadsData& threadsData)
{
	std::vector<std::thread*>&	threads{ threadsData.threads };

	for (auto&& thread : threads)
	{
		thread->join();
	}
}


void 
FiberTaskSchedulerFunction::_Clear(
	FiberTaskSchedulerData& scheduler)
{
	ThreadsData&	threadsData{ scheduler.threadsData };
	std::vector<std::thread*>&	threads{ threadsData.threads };

	for (auto&& thread : threads)
	{
		delete thread;
	}

	threads.clear();
	threadsData.threadsShutdownState.clear();
	threadsData.threadsKeepRunning = true;
	threadsData.threadsBarrier = true;
	threadsData.threadsEmitError = false;
	threadsData.threadsCountSignal = 0u;

	scheduler.threadFibersData.clear();
	scheduler.fibersData.clear();
	scheduler.fibers.clear();
	scheduler.threadsFibers.clear();

	scheduler.fenceMaxCount = 0u;
	scheduler.freeFencesIndices.clear();
	scheduler.fences.clear();

	FiberPoolFunction::Clear(scheduler.fiberPool);
	TaskPoolFunction::Shutdown(scheduler.taskPool);
}


}