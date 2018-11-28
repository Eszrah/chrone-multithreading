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
	FiberPool&	fiberPool{ scheduler.fiberPool };
	std::vector<Fiber>&	fibers{ scheduler.fibers };
	std::vector<FiberData>&	fibersData{ scheduler.fibersData };
	std::vector<std::thread*>&	threads{ threadsData.threads };

	threadsData.threadsKeepRunning = true;
	threadsData.threadsBarrier = true;
	threadsData.threadsEmitError = false;
	threadsData.threadsCountSignal = 0u;

	threads.resize(threadCount);
	fibersData.reserve(threadCount + fiberCount);
	fibers.reserve(fiberCount);
	
	FiberPoolFunction::Reserve(fiberPool, fiberCount);

	for (Uint index{ 0u }; index < threadCount + fiberCount; ++index)
	{
		fibersData.emplace_back(0xFFFFFFFF, &scheduler);
	}

	for (Uint index{ 0u }, fiberDataIndex{ threadCount }; index < fiberCount; ++index, ++fiberDataIndex)
	{
		fibers.emplace_back(WindowsFiberHelper::AllocateHFiber(0u,
			WorkerFiberEntryPoint, &fibersData[fiberDataIndex])); //TO BE FIXED
	}


	//Copying fibers ptr to free fibers array
	std::vector<Fiber*>&	freeFibers{ fiberPool.freeFibers };
	Fiber*	fibersPtr{ fibers.data() };

	freeFibers.resize(fiberCount);
	for (auto index{ 0u }; index < fiberCount; ++index, fibersPtr++)
	{
		freeFibers[index] = fibersPtr;
	}


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
		_JoinAndDeleteThreads(scheduler.threadsData);
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
	return false;
}

void FiberTaskSchedulerFunction::_WaitAnddResetCounter(std::atomic<Uint>& counter, Uint count)
{
	while (counter != count);
	counter = 0u;
}

void FiberTaskSchedulerFunction::_JoinAndDeleteThreads(ThreadsData& threadsData)
{
	std::vector<std::thread*>&	threads{ threadsData.threads };

	for (auto&& thread : threads)
	{
		thread->join();
		delete thread;
	}
}


}