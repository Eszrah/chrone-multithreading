#include "scheduler/FiberTaskSchedulerFunction.h"
#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberPoolFunction.h"
#include "scheduler/WindowsFiberHelper.h"
#include "scheduler/WorkerThreadEntryPoint.h"
#include "scheduler/WorkerFiberEntryPoint.h"

#include <algorithm>

namespace chrone::multithreading::scheduler
{

bool
FiberTaskSchedulerFunction::Initialize(
	FiberTaskSchedulerData& scheduler,
	const Uint threadCount, 
	const Uint fiberCount)
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


	threadsData.threadsKeepRunning = true;
	threadsData.threadsBarrier = true;
	threadsData.threadsEmitError = false;
	threadsData.threadsCountSignal = 0u;
	threadsData.threadsShutdownState.resize(threadCount, false);

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

void FiberTaskSchedulerFunction::_WaitAnddResetCounter(std::atomic<Uint>& counter, Uint count)
{
	while (counter != count);
	counter = 0u;
}

void FiberTaskSchedulerFunction::_JoinThreads(ThreadsData& threadsData)
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
	FiberPoolFunction::Clear(scheduler.fiberPool);
}


}