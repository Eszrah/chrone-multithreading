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
#include "scheduler/SyncPrimitive.h"

#include "std_extension/SpinlockStdExt.h"
#include "AssertMacro.h"

namespace chrone::multithreading::scheduler
{

bool
FiberTaskSchedulerFunction::Initialize(
	FiberTaskSchedulerData& scheduler,
	const Uint32 threadCount, 
	const Uint32 fiberCount,
	const Uint32 maxTaskCountPowerOfTwo,
	const Uint16 fenceMaxCount,
	const Uint16 semaphroeMaxCount)
{
	if (!threadCount ||
		!fiberCount || fiberCount < threadCount ||
		!(maxTaskCountPowerOfTwo && 
			((maxTaskCountPowerOfTwo & (maxTaskCountPowerOfTwo - 1u)) == 0u)))
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

	const Uint32	totalMaxFenceCount{ fenceMaxCount + 1u };
	scheduler.fenceMaxCount = totalMaxFenceCount;
	scheduler.fences = new Fence[totalMaxFenceCount];
	scheduler.freeFencesIndices.resize(totalMaxFenceCount);
	std::iota(scheduler.freeFencesIndices.begin(), 
		scheduler.freeFencesIndices.end(), 
		FiberTaskSchedulerData::defaultHSyncPrimitive);

	const Uint32	totalMaxSemaphoreCount{ 
		semaphroeMaxCount + totalMaxFenceCount + 1u };
	scheduler.allocatedNativeSemaphore = 0u;
	scheduler.semaphoreMaxCount = totalMaxSemaphoreCount;
	scheduler.semaphores = new Semaphore[totalMaxSemaphoreCount];
	scheduler.freeSemaphoresIndices.resize(totalMaxSemaphoreCount);
	std::iota(scheduler.freeSemaphoresIndices.begin(),
		scheduler.freeSemaphoresIndices.end(), 
		FiberTaskSchedulerData::defaultHSyncPrimitive);

	threads.resize(threadCount);
	threadsFibers.resize(threadCount);
	threadFibersData.resize(threadCount);
	fibersData.reserve(threadCount + fiberCount);
	fibers.reserve(fiberCount);

	TaskPoolFunction::Initialize(scheduler.taskPool, maxTaskCountPowerOfTwo);


	Fiber*	threadsFiber{ threadsFibers.data() };
	Fiber*	fiber{ fibers.data() };
	
	FiberPoolFunction::Reserve(fiberPool, fiberCount);

	for (Uint index{ 0 }; index < threadCount; ++index)
	{
		fibersData.emplace_back(0xFFFFFFFF, threadsFiber, &scheduler);
		++threadsFiber;
	}

	for (Uint index{ 0 }; index < fiberCount; ++index)
	{
		fibersData.emplace_back(0xFFFFFFFF, fiber, &scheduler);
		++fiber;
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
	you have to make sure that all fiber are available (just an assert), otherwise some fiber could not be reachable (if a native Fiber is not, could be fucked up)

	ThreadsData&	threadsData{ scheduler.threadsData };
	const auto	threadCount{ threadsData.threads.size() };

	//Signaling to all threads it is time to exit
	threadsData.threadsBarrier.store(true, std::memory_order_release);
	threadsData.threadsCountSignal.store(0u, std::memory_order_release);
	threadsData.threadsKeepRunning.store(false, std::memory_order_release);
	//Wait all threads have pushed their old fiber and reached the _threadsBarrier
	_WaitAnddResetCounter(threadsData.threadsCountSignal, static_cast<Uint>(threadCount));
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
	return  _AllocateFence(scheduler);
}


void
FiberTaskSchedulerFunction::FreeFence(
	FiberTaskSchedulerData& scheduler,
	HFence hFence)
{
	_FreeFence(scheduler, hFence);
}


HSemaphore 
FiberTaskSchedulerFunction::AllocateSemaphore(
	FiberTaskSchedulerData& scheduler)
{
	return HSemaphore{ _AllocateSemaphore(scheduler) };
}


void 
FiberTaskSchedulerFunction::FreeSemaphre(
	FiberTaskSchedulerData& scheduler, 
	HSemaphore semaphore)
{
	_FreeSemaphore(scheduler, semaphore);
}


HFence
FiberTaskSchedulerFunction::_AllocateFence(
	FiberTaskSchedulerData& scheduler)
{
	Fence*	fences{ scheduler.fences };
	Uint32	fenceIndex{ };

	{
		LockGuardSpinLock	lock{ scheduler.fenceLock };
		std::vector<Uint32>&	freeFencesIndices{ scheduler.freeFencesIndices };

		//must handle if too many allocations

		fenceIndex = freeFencesIndices.back();
		freeFencesIndices.pop_back();
	}
	fences[fenceIndex].hSemaphore = _AllocateSemaphore(scheduler);

	return HFence{ fenceIndex };
}


void
FiberTaskSchedulerFunction::_FreeFence(
	FiberTaskSchedulerData& scheduler,
	HFence hFence)
{
	const Fence*	fences{ scheduler.fences };

	_FreeSemaphore(scheduler, fences[hFence.handle].hSemaphore);

	{
		LockGuardSpinLock	lock{ scheduler.fenceLock };
		std::vector<Uint32>&	freeFencesIndices{ scheduler.freeFencesIndices };

		freeFencesIndices.push_back(hFence.handle);
	}
}


HSemaphore
FiberTaskSchedulerFunction::_AllocateSemaphore(
	FiberTaskSchedulerData& scheduler)
{

	LockGuardSpinLock	lock{ scheduler.semaphoreLock };
	std::vector<Uint>&	freeSemaphoresIndices{ scheduler.freeSemaphoresIndices };

	//must handle if too many allocations
	++scheduler.allocatedNativeSemaphore;
	const Uint32	semaphoreIndex{ freeSemaphoresIndices.back() };
	freeSemaphoresIndices.pop_back();

	return HSemaphore{ semaphoreIndex };
}


void 
FiberTaskSchedulerFunction::_FreeSemaphore(
	FiberTaskSchedulerData& scheduler, 
	HSemaphore hSemaphore)
{
	LockGuardSpinLock	lock{ scheduler.semaphoreLock };
	std::vector<Uint32>&	freeSemaphoresIndices{ scheduler.freeSemaphoresIndices };

	freeSemaphoresIndices.push_back(hSemaphore.handle);
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
	delete[] scheduler.fences;
	scheduler.fences = nullptr;

	scheduler.allocatedNativeSemaphore = 0u;
	scheduler.semaphoreMaxCount = 0u;
	scheduler.freeSemaphoresIndices.clear();
	delete[] scheduler.semaphores;
	scheduler.semaphores = nullptr;

	FiberPoolFunction::Clear(scheduler.fiberPool);
	TaskPoolFunction::Clear(scheduler.taskPool);
}


}