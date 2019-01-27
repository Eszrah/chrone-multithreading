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

namespace chrone::multithreading::fiberScheduler
{

bool
TaskSchedulerFunction::Initialize(
	TaskSchedulerData& scheduler,
	const Uint16 threadCount, 
	const Uint16 fiberCount,
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

	threadsData.threadsKeepRunning = true;
	threadsData.threadsBarrier = true;
	threadsData.threadsEmitError = false;
	threadsData.threadsCountSignal = 0u;
	threadsData.threadsShutdownState.resize(threadCount, false);

	//+1 because we will use the first one as the default fence
	scheduler.fences = new Fence[fenceMaxCount + 1u];
	scheduler.freeFencesIndices.resize(fenceMaxCount);
	std::iota(scheduler.freeFencesIndices.begin(), 
		scheduler.freeFencesIndices.end(), 
		TaskSchedulerData::defaultHSyncPrimitive + 1u);

	//+1 because we will use the first one as the default semaphore
	const Uint32	totalMaxSemaphoreCount{ 
		semaphroeMaxCount + fenceMaxCount + 1u };
	scheduler.semaphoreMaxCount = totalMaxSemaphoreCount;
	scheduler.semaphores = new Semaphore[totalMaxSemaphoreCount];
	scheduler.freeSemaphoresIndices.resize(totalMaxSemaphoreCount - 1);
	std::iota(scheduler.freeSemaphoresIndices.begin(),
		scheduler.freeSemaphoresIndices.end(), 
		TaskSchedulerData::defaultHSyncPrimitive + 1u);

	threads.resize(threadCount);
	threadsFibers.resize(threadCount);
	threadFibersData.resize(threadCount);
	fibersData.reserve(threadCount + fiberCount);
	fibers.reserve(fiberCount);

	TaskPoolFunction::Initialize(scheduler.taskPool, maxTaskCountPowerOfTwo);


	Fiber*	threadsFiber{ threadsFibers.data() };
	Fiber*	fiber{ fibers.data() };
	
	FiberPoolFunction::Reserve(fiberPool, fiberCount);
	scheduler.nativeFiberCount = fiberCount;

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
		_Clear(scheduler);
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
	threadsData.threadsBarrier.store(false, std::memory_order_seq_cst);

	if (threadsData.threadsEmitError)
	{
		_JoinThreads(scheduler.threadsData);
		_Clear( scheduler );
		return false;
	}

	//making sure they all have passed the barrier
	_WaitAnddResetCounter(threadsData.threadsCountSignal, threadCount);

	return true;
}


bool
TaskSchedulerFunction::Shutdown(
	TaskSchedulerData& scheduler)
{
	ThreadsData&	threadsData{ scheduler.threadsData };
	const Uint	threadCount{ static_cast<Uint>(threadsData.threads.size()) };

	//Signaling to all threads it is time to exit
	threadsData.threadsBarrier.store(true, std::memory_order_seq_cst);
	threadsData.threadsCountSignal.store(0u, std::memory_order_seq_cst);
	threadsData.threadsKeepRunning.store(false, std::memory_order_seq_cst);
	//Wait all threads have pushed their old fiber and reached the _threadsBarrier
	_WaitAnddResetCounter(threadsData.threadsCountSignal, threadCount);
	//Release store to make sure the counter has well been reseted before signaling the fence to false
	threadsData.threadsBarrier.store(false, std::memory_order_seq_cst);

	Uint const	threadCountTwice{ threadCount * 2u };
	while (threadsData.threadsCountSignal.load(std::memory_order_seq_cst ) 
		!= threadCountTwice);
	
	threadsData.threadsCountSignal.store(0u, std::memory_order_seq_cst);

	_JoinThreads(threadsData);
	_Clear(scheduler);

	return true;
}

HFence
TaskSchedulerFunction::AllocateFence(
	TaskSchedulerData& scheduler)
{
	return  _AllocateFence(scheduler);
}


void
TaskSchedulerFunction::DeallocateFence(
	TaskSchedulerData& scheduler,
	HFence hFence)
{
	_DeallocateFence(scheduler, hFence);
}


HSemaphore 
TaskSchedulerFunction::AllocateSemaphore(
	TaskSchedulerData& scheduler)
{
	return _AllocateSemaphore(scheduler);
}


void 
TaskSchedulerFunction::DeallocateSemaphore(
	TaskSchedulerData& scheduler, 
	HSemaphore semaphore)
{
	_DeallocateSemaphore(scheduler, semaphore);
}


HFence
TaskSchedulerFunction::_AllocateFence(
	TaskSchedulerData& scheduler)
{
	Fence*	fences{ scheduler.fences };
	Uint32	fenceIndex{ };

	{
		LockGuardSpinLock	lock{ scheduler.fenceLock };
		std::vector<Uint32>&	freeFencesIndices{ scheduler.freeFencesIndices };

		if( freeFencesIndices.empty() )
		{
			return HFence{ TaskSchedulerData::invalidHSyncPrimitive };
		}

		fenceIndex = freeFencesIndices.back();
		freeFencesIndices.pop_back();
	}
	fences[fenceIndex].hSemaphore = _AllocateSemaphore(scheduler);

	return HFence{ fenceIndex };
}


void
TaskSchedulerFunction::_DeallocateFence(
	TaskSchedulerData& scheduler,
	HFence hFence)
{
	const Fence*	fences{ scheduler.fences };

	assert(hFence.handle != TaskSchedulerData::defaultHSyncPrimitive &&
		hFence.handle != TaskSchedulerData::invalidHSyncPrimitive);

	_DeallocateSemaphore(scheduler, fences[hFence.handle].hSemaphore);

	{
		LockGuardSpinLock	lock{ scheduler.fenceLock };
		std::vector<Uint32>&	freeFencesIndices{ scheduler.freeFencesIndices };

		freeFencesIndices.push_back(hFence.handle);
	}
}


HSemaphore
TaskSchedulerFunction::_AllocateSemaphore(
	TaskSchedulerData& scheduler)
{

	LockGuardSpinLock	lock{ scheduler.semaphoreLock };
	std::vector<Uint>&	freeSemaphoresIndices{ scheduler.freeSemaphoresIndices };

	if(freeSemaphoresIndices.empty())
	{
		return HSemaphore{ TaskSchedulerData::invalidHSyncPrimitive };
	}

	const Uint32	semaphoreIndex{ freeSemaphoresIndices.back() };
	freeSemaphoresIndices.pop_back();

	return HSemaphore{ semaphoreIndex };
}


void 
TaskSchedulerFunction::_DeallocateSemaphore(
	TaskSchedulerData& scheduler, 
	HSemaphore hSemaphore)
{
	LockGuardSpinLock	lock{ scheduler.semaphoreLock };
	std::vector<Uint32>&	freeSemaphoresIndices{ scheduler.freeSemaphoresIndices };

	assert(hSemaphore.handle != TaskSchedulerData::defaultHSyncPrimitive &&
		hSemaphore.handle != TaskSchedulerData::invalidHSyncPrimitive);
	freeSemaphoresIndices.push_back(hSemaphore.handle);
}


void
TaskSchedulerFunction::_WaitAnddResetCounter(
	std::atomic<Uint>& counter, 
	Uint count)
{
	while (counter.load(std::memory_order_seq_cst) != count);
	counter.store(0u, std::memory_order_seq_cst );
}


void 
TaskSchedulerFunction::_JoinThreads(
	ThreadsData& threadsData)
{
	std::vector<std::thread*>&	threads{ threadsData.threads };

	for (auto&& thread : threads)
	{
		thread->join();
	}
}


void 
TaskSchedulerFunction::_Clear(
	TaskSchedulerData& scheduler)
{
	ThreadsData&	threadsData{ scheduler.threadsData };
	std::vector<std::thread*>&	threads{ threadsData.threads };

	{
		LockGuardSpinLock	lock{ scheduler.fiberPool.freeFibersLock };
		assert(scheduler.fiberPool.freeFibers.size() == scheduler.nativeFiberCount + threads.size());
	}

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

	scheduler.freeFencesIndices.clear();
	delete[] scheduler.fences;
	scheduler.fences = nullptr;

	scheduler.semaphoreMaxCount = 0u;
	scheduler.freeSemaphoresIndices.clear();
	delete[] scheduler.semaphores;
	scheduler.semaphores = nullptr;

	FiberPoolFunction::Clear(scheduler.fiberPool);
	TaskPoolFunction::Clear(scheduler.taskPool);
	scheduler.nativeFiberCount = 0u;
}


}