#include "scheduler/WorkerThreadEntryPoint.h"

namespace chrone::multithreading::scheduler
{

void 
WorkerThreadFunction::EntryPoint(
	WorkerThreadFuncData funcData)
{
	if (!_Initialize(funcData)) { return; }

	//while (_threadsKeepRunning)
	//{
	//	_ExecuteAndTryFindFiber();
	//	//If no tasks, wait
	//}

	_Shutdown(funcData);
}

bool 
WorkerThreadFunction::_Initialize(
	WorkerThreadFuncData funcData)
{
	//FiberThreadData&	fiberThreadData{ _fiberThreadData[threadIndex] };
	//void*	threadFiberHandle{
	//	WindowsFiberHelper::ConvertThreadToFiber(nullptr) };
	//if (!threadFiberHandle)
	//{
	//	_threadsEmitError.store(true, std::memory_order_relaxed);
	//	_threadsCountSignal.fetch_add(1, std::memory_order_release);
	//	return false;
	//}
	//idArray[threadIndex] = std::this_thread::get_id();
	//localThreadIndex = threadIndex;
	//Fiber&	threadFiber{ fiberThreadData.threadFiber };
	//threadFiber.fiberHandle = threadFiberHandle;
	//fiberThreadData.currentThreadFiber = std::addressof(threadFiber);
	//if (threadIndex == 0u)
	//{
	//	std::vector<Fiber>&	fibers{ _fiberPool.fibers };
	//	FiberData*	fiberDataPtr{ std::addressof(_fibersData) };
	//	for (Uint index{ 0u }; index < _fiberCount; ++index)
	//	{
	//		fibers.emplace_back(WindowsFiberHelper::AllocateHFiber(0u,
	//			&FiberEntryPoint, fiberDataPtr));
	//	}
	//	_fiberPool.MakeAllFibersFree();

	//	bool const	checkFibersValid{ std::all_of(fibers.cbegin(), fibers.cend(),
	//		[](Fiber const& fiber) { return fiber.fiberHandle != nullptr; }) };
	//	if (!checkFibersValid)
	//	{
	//		_fiberPool.Clear();
	//		_threadsEmitError.store(true, std::memory_order_relaxed);
	//		_threadsCountSignal.fetch_add(1, std::memory_order_release);
	//		return false;
	//	}
	//}
	////signal everything is fine and wait
	//_threadsCountSignal.fetch_add(1, std::memory_order_release);
	//while (_threadsBarrier.load(std::memory_order_acquire));
	////signaling we have passed the barrier
	//_threadsCountSignal.fetch_add(1, std::memory_order_release);
	//return !_threadsEmitError.load(std::memory_order_acquire);
	return false;
}

bool 
WorkerThreadFunction::_Shutdown(
	WorkerThreadFuncData funcData)
{
	return false;
}

}
