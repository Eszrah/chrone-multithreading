#include "scheduler/fiber/FiberTaskScheduler.h"

#include "scheduler/fiber/WindowsFiberHelper.h"
#include "scheduler/fiber/FiberEntryPoint.h"
#include "scheduler/fiber/TaskSyncPrimitiveData.h"

#include <algorithm>
#include <cassert>

namespace chrone::multithreading::scheduler
{

FiberTaskScheduler::FiberTaskScheduler()
{
}


bool 
FiberTaskScheduler::Initialize(
	Uint threadCount, 
	Uint fiberCount)
{
	if (_IsInit() || !threadCount || 
		!fiberCount || fiberCount < threadCount) 
	{ 
		return false; 
	}

	return _Initialize(threadCount, fiberCount);
}


bool 
FiberTaskScheduler::Shutdown()
{
	if (!_IsInit()) { return false; }
	return _Shutdown();
}


//bool
//FiberTaskScheduler::AllocateSyncPrimitive(
//	SyncPrimitive& syncPrimitive)
//{
//	syncPrimitive.data = _AllocateSyncPrimitiveData();
//	return true;
//}
//
//
//bool
//FiberTaskScheduler::DeallocateSyncPrimitive(
//	SyncPrimitive& syncPrimitive)
//{
//	SyncPrimitiveData*	syncPrimitiveData{
//		static_cast<SyncPrimitiveData*>(syncPrimitive.data) };
//	if (!syncPrimitiveData) { return false; }
//	_DeallocateSyncPrimitiveData(syncPrimitiveData);
//	syncPrimitive.data = nullptr;
//	return true;
//}


bool 
FiberTaskScheduler::PushTasks(
	Uint count, 
	TaskDecl* tasks, 
	SyncPrimitive* sync)
{
	if (!count || !tasks) { return false; }

	//SyncPrimitiveData*	syncData{ nullptr };

	//SyncPrimitiveData* syncPrimitiveData{
	//	sync ? static_cast<SyncPrimitiveData*>(sync->data) : nullptr };

	//if (sync)
	//{
	//	assert(syncPrimitiveData);
	//}

	//return _AllocateAndPushTasks(count, tasks, nullptr, 
	//	std::addressof(syncPrimitiveData->counter));
}


//bool
//FiberTaskScheduler::Wait(
//	SyncPrimitive& prim)
//{
//	SyncPrimitiveData* syncPrimitiveData{
//		static_cast<SyncPrimitiveData*>(prim.data) };
//	assert(syncPrimitiveData);
//	if (!syncPrimitiveData) { return false; }
//
//	while (syncPrimitiveData->counter.load(std::memory_order_acquire) != 0)
//	{
//		std::this_thread::yield();
//	}
//
//	return true;
//}


//bool 
//FiberTaskScheduler::CheckFinished(
//	SyncPrimitive& prim)
//{
//	return false;
//}
//

void 
FiberTaskScheduler::_WaitThreads()
{
	auto const threadCount{ _threads.size() };
	while (_threadsCountSignal != threadCount);
}


void 
FiberTaskScheduler::_WaitThreadsAndResetCount()
{
	_WaitThreads();
	_threadsCountSignal = 0u;
}


bool 
FiberTaskScheduler::_Initialize(
	Uint threadCount, 
	Uint fiberCount)
{
	//_threadsKeepRunning = true;
	//_threadsBarrier = true;
	//_threadsEmitError = false;
	//_threadsCountSignal = 0u;

	//_threads.resize(threadCount);
	//_fiberThreadData.resize(threadCount);

	//Uint const	totalFiberCount{ fiberCount + threadCount };
	//_fiberCount = fiberCount;
	//_fiberPool.Reserve(fiberCount, totalFiberCount);

	//std::vector<Fiber>&	fibers{ _fiberPool.fibers };
	//FiberFuncData*	fiberDataPtr{ std::addressof(_fibersFuncData) };
	//for (Uint index{ 0u }; index < fiberCount; ++index)
	//{
	//	fibers.emplace_back(WindowsFiberHelper::AllocateHFiber(0u,
	//		&FiberEntryPoint, fiberDataPtr));
	//}
	//_fiberPool.MakeAllFibersFree();

	//bool const	checkFibersValid{ std::all_of(fibers.cbegin(), fibers.cend(),
	//	[](Fiber const& fiber) { return fiber.fiberHandle != nullptr; }) };

	//if (!checkFibersValid)
	//{
	//	_fiberPool.Clear();
	//	return false;
	//}

	//for (auto index{ 0u }; index < threadCount; ++index)
	//{
	//	_threads[index] = new std::thread(&FiberTaskScheduler::_WorkerThreadFunction, 
	//		this, index);
	//}

	////Waiting all threads have finished initialization
	//_WaitThreadsAndResetCount();
	//_threadsKeepRunning = _threadsEmitError ? false : true;
	//_threadsBarrier.store(false, std::memory_order_release);

	//if (_threadsEmitError)
	//{
	//	_JoinThreads();
	//	_fiberPool.Clear();
	//	return false;
	//}

	////making sure they all have passed the barrier
	//_WaitThreadsAndResetCount();

	//_state = INIT;
	return true;
}


bool 
FiberTaskScheduler::_Shutdown()
{
	if (_IsInit())
	{
		_ShutdownThreads();
	}

	_state = CREATED;
	_threadsKeepRunning = true;
	_threadsBarrier = true;
	_threadsEmitError = false;
	_threadsCountSignal = 0u;

	_threads.clear();
	_fiberThreadData.clear();
	//_fiberPool.Clear();

	return true;
}


bool 
FiberTaskScheduler::_ShutdownThreads()
{
	//Signaling to all threads it is time to exit
	_threadsBarrier.store(true, std::memory_order_release);
	_threadsCountSignal.store(0u, std::memory_order_release);
	_threadsKeepRunning.store(false, std::memory_order_release);

	//Wait all threads have pushed their old fiber and reeach the _threadsBarrier
	_WaitThreadsAndResetCount();
	_threadsBarrier.store(false, std::memory_order_release);
	auto const threadCountTwice{ _threads.size() * 2u };
	while (_threadsCountSignal != threadCountTwice);
	_threadsCountSignal.store(0u, std::memory_order_release);

	return _JoinThreads();
}


bool 
FiberTaskScheduler::_JoinThreads()
{
	for (auto&& thread : _threads)
	{
		thread->join();
		delete thread;
	}
	return true;
}


void 
FiberTaskScheduler::_WorkerThreadFunction(
	Uint threadIndex)
{
	if (!_WorkerThreadInit(threadIndex)) { return; }

	while (_threadsKeepRunning)
	{
		_ExecuteAndTryFindFiber();
		//If no tasks, wait
	}

	_WorkerThreadShutdown(threadIndex);
}


bool 
FiberTaskScheduler::_WorkerThreadInit(
	Uint threadIndex)
{
	FiberThreadData&	fiberThreadData{ _fiberThreadData[threadIndex] };

	void*	threadFiberHandle{
		WindowsFiberHelper::ConvertThreadToFiber(nullptr) };

	if (!threadFiberHandle)
	{
		_threadsEmitError.store(true, std::memory_order_relaxed);
		_threadsCountSignal.fetch_add(1, std::memory_order_release);
		return false;
	}

	Fiber&	threadFiber{ fiberThreadData.threadFiber };
	threadFiber.fiberHandle = threadFiberHandle;
	fiberThreadData.currentThreadFiber = std::addressof(threadFiber);

	if (threadIndex == 0u)
	{
		std::vector<Fiber>&	fibers{ _fiberPool.fibers };
		FiberFuncData*	fiberFuncDataPtr{ std::addressof(_fibersFuncData) };
		for (Uint index{ 0u }; index < _fiberCount; ++index)
		{
			fibers.emplace_back(WindowsFiberHelper::AllocateHFiber(0u,
				&FiberEntryPoint, fiberFuncDataPtr));
		}
		_fiberPool.MakeAllFibersFree();
		
		bool const	checkFibersValid{ std::all_of(fibers.cbegin(), fibers.cend(),
			[](Fiber const& fiber) { return fiber.fiberHandle != nullptr; }) };

		if (!checkFibersValid)
		{
			_fiberPool.Clear();
			_threadsEmitError.store(true, std::memory_order_relaxed);
			_threadsCountSignal.fetch_add(1, std::memory_order_release);
			return false;
		}
	}

	//signal everything is fine and wait
	_threadsCountSignal.fetch_add(1, std::memory_order_release);
	while (_threadsBarrier.load(std::memory_order_acquire));

	//signaling we have passed the barrier
	_threadsCountSignal.fetch_add(1, std::memory_order_release);
	return !_threadsEmitError.load(std::memory_order_acquire);
}


bool 
FiberTaskScheduler::_WorkerThreadShutdown(
	Uint orioginalThreadIndex)
{
	_WorkerThreadShutdownSwitchToNativeFiber();
	return orioginalThreadIndex == localThreadIndex;
}


void 
FiberTaskScheduler::_WorkerThreadShutdownSwitchToNativeFiber()
{
	FiberThreadData&	fiberThreadData{ _fiberThreadData[localThreadIndex] };

	if (!fiberThreadData.hasShutdown)
	{
		//Pushing the old fiber if there is one, could contains another native thread fiber or a classic fiber
		_PushPreviousThreadFiber(fiberThreadData);

		//Making sure everybody have free its old fiber
		_threadsCountSignal.fetch_add(1, std::memory_order_release);
		while (_threadsBarrier.load(std::memory_order_acquire));

		//signaling we have passed the barrier
		_threadsCountSignal.fetch_add(1, std::memory_order_release);

		const auto	findNativeFiberL =
			[fiberThreadData = std::cref(_fiberThreadData)]
		(Fiber* fiber)
		{
			auto const&	contianer{ fiberThreadData.get() };
			return std::none_of(contianer.cbegin(), contianer.cend(), 
				[fiber](FiberThreadData const& threadData) 
			{ return threadData.threadFiber.fiberHandle == fiber->fiberHandle; 
			});
		};

		//Getting and Switching to a native fiber
		Fiber*	nativeFiber{ _fiberPool.GetFreeFiber(findNativeFiberL) };
		assert(nativeFiber);
		fiberThreadData.hasShutdown = true;
		_SwitchToFiber(nativeFiber);
	}
}


void
FiberTaskScheduler::_SwitchToFiber(Fiber* newFiber)
{
	{
	//assert(std::addressof(fiberThreadData) == std::addressof(_fiberThreadData[localThreadIndex]));
	FiberThreadData&	fiberThreadData{ _fiberThreadData[localThreadIndex] };
	//assert(newFiber->fiberHandle == WindowsFiberHelper::GetCurrentFiber());
	//while (newFiber->fiberHandle == WindowsFiberHelper::GetCurrentFiber());
	fiberThreadData.previousThreadFiber = fiberThreadData.currentThreadFiber;
	fiberThreadData.currentThreadFiber = newFiber;
	//assert(fiberThreadData.previousThreadFiber);
	//while (!fiberThreadData.previousThreadFiber);

	//localPreviousFiber = fiberThreadData.previousThreadFiber;
	//localCurrentFiber = fiberThreadData.currentThreadFiber;

	assert(newFiber->fiberHandle);
	WindowsFiberHelper::SwitchToFiber(newFiber->fiberHandle);
	}
	FiberThreadData&	fiberThreadData{ _fiberThreadData[localThreadIndex] };
	TaskSyncPrimitiveData*	waitingSyncPrimitive{ 
		fiberThreadData.waitingSyncPrimitive };

	//if (waitingSyncPrimitive)
	//{
	//	//checking if This thread is the last one which has a valid link to the dependent fiber
	//	if (waitingSyncPrimitive->dependencyCoouter.fetch_sub(1u,
	//		std::memory_order_release) == 1u)
	//	{
	//	//	_fiberPool.PushWaitingFiber(waitingSyncPrimitive->dependencyFiber);
	//	}

	//	fiberThreadData.waitingSyncPrimitive = nullptr;
	//}

	_PushPreviousThreadFiber(fiberThreadData);
}


void
FiberTaskScheduler::_PushPreviousThreadFiber(
	FiberThreadData& fiberThreadData)
{
	if (fiberThreadData.previousThreadFiber)
	{
		_fiberPool.PushFreeFiber(fiberThreadData.previousThreadFiber);
		fiberThreadData.previousThreadFiber = nullptr;
	}
}


bool 
FiberTaskScheduler::_AllocateAndPushTasks(
	Uint count, 
	TaskDecl* tasks, 
	std::atomic<Fiber*>* dependencyFiber,
	AtomicCounter* dependencyCoouter)
{
	if (dependencyCoouter) 
	{ 
		dependencyCoouter->store(count, std::memory_order_relaxed); 
	}

	return _fiberPool.PushReadyTasks(count, tasks, dependencyFiber, dependencyCoouter);
}


bool
FiberTaskScheduler::_ExecuteAndTryFindFiber()
{
	FiberThreadData&	fiberThreadData{ _fiberThreadData[localThreadIndex] };
	Task&	currentThreadFiberTask{ fiberThreadData.currentThreadFiber->task };

	bool const	isTerminalFiber{ currentThreadFiberTask.CheckedExecute() };
	Fiber*	newFiber{ isTerminalFiber ?
		currentThreadFiberTask.GetDependencyTask() : nullptr };

	currentThreadFiberTask.Reset();

	if (_threadsKeepRunning.load(std::memory_order_acquire))
	{
		if (!newFiber && !_fiberPool.TryPopWaitingFiber(newFiber))
		{
			//Try to hook a ready fiber
			//_fiberPool.TryPopReadyFiber(newFiber);
			_fiberPool.TryPopFreeFiber(newFiber);
		}

		if (!newFiber) { return false; }
		_SwitchToFiber(newFiber);
	}

	_PushPreviousThreadFiber(_fiberThreadData[localThreadIndex]);

	return true;
}


FiberTaskScheduler::SyncPrimitiveData*
FiberTaskScheduler::_AllocateSyncPrimitiveData()
{
	std::lock_guard<Spinlock>	guard{ _syncPrimitiveDataLock };
	return new SyncPrimitiveData();
}


void 
FiberTaskScheduler::_DeallocateSyncPrimitiveData(
	SyncPrimitiveData* syncPrimitiveData)
{
	std::lock_guard<Spinlock>	guard{ _syncPrimitiveDataLock };
	delete syncPrimitiveData;
}


TaskSyncPrimitiveData*
FiberTaskScheduler::_AllocateTaskSyncPrimitiveData()
{
	std::lock_guard<Spinlock>	guard{ _taskSyncPrimitiveDataLock };
	return new TaskSyncPrimitiveData();
}


void FiberTaskScheduler::
_DeallocateTaskSyncPrimitiveData(
	TaskSyncPrimitiveData* syncPrimitiveData)
{
	std::lock_guard<Spinlock>	guard{ _taskSyncPrimitiveDataLock };
	delete syncPrimitiveData;
}


//void 
//FiberTaskScheduler::_ProxyExecuteTask()
//{
//	_ExecuteAndTryFindFiber();
//}
//
//
//__declspec(noinline) void
//FiberTaskScheduler::_ProxyInitFiberFirstEntry()
//{
//	FiberThreadData&	fiberThreadData{ _fiberThreadData[localThreadIndex] };
//	//assert(idArray[localThreadIndex] == std::this_thread::get_id());
//	//assert(fiberThreadData.previousThreadFiber == localPreviousFiber);
//	assert(fiberThreadData.previousThreadFiber);
//	//while (!fiberThreadData.previousThreadFiber);
//	_fiberPool.PushFreeFiber(fiberThreadData.previousThreadFiber);
//	fiberThreadData.previousThreadFiber = nullptr;
//}
//
//
//void 
//FiberTaskScheduler::_ProxySwitchBackToOriginalThread()
//{
//	//SHOULD NOT BE EXECUTED TWICE !!!!!
//
//	FiberThreadData&	fiberThreadData{ _fiberThreadData[localThreadIndex] };
//
//	if (!fiberThreadData.hasShutdown)
//	{
//		//Pushing the old fiber if there is one, could contains another native thread fiber or a classic fiber
//		_PushPreviousThreadFiber(fiberThreadData);
//		fiberThreadData.hasShutdown = true;
//
//		//Making sure everybody have free its old fiber
//		_threadsCountSignal.fetch_add(1, std::memory_order_release);
//		while (_threadsBarrier.load(std::memory_order_acquire));
//
//		//signaling we have passed the barrier
//		_threadsCountSignal.fetch_add(1, std::memory_order_release);
//	}
//
//	//Waiting for all other threads to complete their operations
//	_threadsCountSignal.fetch_add(1, std::memory_order_release);
//	while (_threadsCountSignal.load(std::memory_order_acquire) != 0u);
//
//	//Switching to the thread original fiber
//	_SwitchToFiber(std::addressof(fiberThreadData.threadFiber));
//	/*WindowsFiberHelper::SwitchToFiber(fiberThreadData.threadFiber.fiberHandle);*/
//}


//bool 
//FiberTaskScheduler::_AgentAllocateSyncPrimitive(
//	TaskSyncPrimitive& syncPrim)
//{
//	syncPrim.data = _AllocateTaskSyncPrimitiveData();
//	return true;
//}
//
//
//bool 
//FiberTaskScheduler::_AgentDeallocateSyncPrimitive(
//	TaskSyncPrimitive& syncPrim)
//{
//	TaskSyncPrimitiveData*	syncPrimitiveData{ 
//		static_cast<TaskSyncPrimitiveData*>(syncPrim.data) };
//	if (!syncPrimitiveData) { return false; }
//
//	_DeallocateTaskSyncPrimitiveData(syncPrimitiveData);
//	syncPrim.data = nullptr;
//	return true;
//}
//
//
//bool
//FiberTaskScheduler::_AgentPushTasks(
//	Uint count, 
//	TaskDecl* tasks, 
//	TaskSyncPrimitive* syncPrim)
//{
//	if (!count || !tasks) { return false; }
//	TaskSyncPrimitiveData* syncPrimitiveData{ syncPrim ? 
//		static_cast<TaskSyncPrimitiveData*>(syncPrim->data) : nullptr };
//
//	if (syncPrim)
//	{
//		assert(syncPrimitiveData);
//	}
//
//	return _AllocateAndPushTasks(count, tasks, nullptr, 
//		std::addressof(syncPrimitiveData->dependencyCoouter));
//}
//
//
//bool 
//FiberTaskScheduler::_AgentWait(
//	TaskSyncPrimitive& syncPrim)
//{
//	//FiberThreadData&	fiberThreadData{ _fiberThreadData[localThreadIndex] };
//	//Fiber*	currentThreadFiber{ fiberThreadData.currentThreadFiber };
//
//	//TaskSyncPrimitiveData* syncPrimitiveData{ 
//	//	static_cast<TaskSyncPrimitiveData*>(syncPrim.data) };
//	//assert(syncPrimitiveData);
//
//	////ADD an if to prevent user mistake ? + a free ?
//	//auto&	atomicDependencyCounter{ syncPrimitiveData->dependencyCoouter };
//	//atomicDependencyCounter.fetch_add(1u,
//	//	std::memory_order_release);
//
//	//syncPrimitiveData->dependencyFiber.store(currentThreadFiber, 
//	//	std::memory_order_release);
//
//	//Fiber*	newFiber{ nullptr };
//
//	//while (atomicDependencyCounter.load(std::memory_order_acquire) != 1u)
//	//{
//	//	if (!_fiberPool.TryPopWaitingFiber(newFiber))
//	//	{
//	//		_fiberPool.TryPopReadyFiber(newFiber);
//	//	}
//
//	//	if (!newFiber) { continue; }
//	//	fiberThreadData.waitingSyncPrimitive = syncPrimitiveData;
//	//	_SwitchToFiber(newFiber);
//	//	assert(atomicDependencyCounter.load(std::memory_order_acquire) == 0u);
//	//}
//
//	return true;
//}
//
//
//bool 
//FiberTaskScheduler::_AgentCheckFinished(
//	TaskSyncPrimitive& syncPrim)
//{
//	TaskSyncPrimitiveData* syncPrimitiveData{
//		static_cast<TaskSyncPrimitiveData*>(syncPrim.data) };
//	assert(syncPrimitiveData);
//
//	auto&	atomicDependencyCounter{ syncPrimitiveData->dependencyCoouter };
//	return (atomicDependencyCounter.load(std::memory_order_acquire) == 0u);
//}

}
