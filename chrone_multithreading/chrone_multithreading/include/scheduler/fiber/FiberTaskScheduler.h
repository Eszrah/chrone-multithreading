#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include "NativeType.h"
#include "FiberPool.h"
#include "FiberFuncData.h"
#include "TaskDecl.h"
#include "FiberTaskSchedulerAgent.h"

namespace chrone::multithreading::scheduler
{

struct TaskDecl;
struct TaskSyncPrimitive;
struct TaskSyncPrimitiveData;

class FiberTaskScheduler
{
public:

	struct SyncPrimitive
	{
		void*	data{ nullptr };
	};

	FiberTaskScheduler();
	FiberTaskScheduler(const FiberTaskScheduler&) = delete;
	FiberTaskScheduler(FiberTaskScheduler&&) = delete;
	~FiberTaskScheduler() = default;

	FiberTaskScheduler&	operator=(const FiberTaskScheduler&) = delete;
	FiberTaskScheduler&	operator=(FiberTaskScheduler&&) = delete;

	bool	Initialize(Uint threadCount, Uint fiberCount);
	bool	Shutdown();

	//bool	AllocateSyncPrimitive(SyncPrimitive& syncPrimitive);
	//bool	DeallocateSyncPrimitive(SyncPrimitive& syncPrimitive);
	bool	PushTasks(Uint count, TaskDecl* tasks, SyncPrimitive* sync = nullptr);
	//bool	Wait(SyncPrimitive& prim);
	//bool	CheckFinished(SyncPrimitive& prim);

private:

	//using AtomicCounter = FiberPool::AtomicCounter;

	struct FiberThreadData
	{
		FiberThreadData() = default;
		FiberThreadData(const FiberThreadData&) = delete;
		FiberThreadData(FiberThreadData&& other) = default;
		~FiberThreadData() = default;

		FiberThreadData&	operator=(const FiberThreadData&) = default;
		FiberThreadData&	operator=(FiberThreadData&& other) = default;

		bool	hasShutdown{ false };
		TaskSyncPrimitiveData*	waitingSyncPrimitive{ nullptr };
		Fiber*	previousThreadFiber{ nullptr };
		Fiber*	currentThreadFiber{ nullptr };
		Fiber	threadFiber{};
	};

	struct SyncPrimitiveData
	{
		//AtomicCounter	counter{};
	};

	enum ESTATE { CREATED, INIT };

	inline bool	_IsInit() const { return _state == INIT; }
	void	_WaitThreads();
	void	_WaitThreadsAndResetCount();
	bool	_Initialize(Uint threadCount, Uint fiberCount);
	bool	_Shutdown();
	bool	_ShutdownThreads();
	bool	_JoinThreads();

	void	_WorkerThreadFunction(Uint threadIndex);
	bool	_WorkerThreadInit(Uint threadIndex);
	bool	_WorkerThreadShutdown(Uint orioginalThreadIndex);
	void	_WorkerThreadShutdownSwitchToNativeFiber();
	
	void	_SwitchToFiber(Fiber* newFiber);
	void	_PushPreviousThreadFiber(FiberThreadData& fiberThreadData);

	//bool	_AllocateAndPushTasks(Uint count, TaskDecl* tasks, std::atomic<Fiber*>* dependencyFiber = nullptr, AtomicCounter* dependencyCoouter = nullptr);
	bool	_ExecuteAndTryFindFiber();

	SyncPrimitiveData*_AllocateSyncPrimitiveData();
	void	_DeallocateSyncPrimitiveData(SyncPrimitiveData* syncPrimitiveData);

	TaskSyncPrimitiveData*_AllocateTaskSyncPrimitiveData();
	void	_DeallocateTaskSyncPrimitiveData(TaskSyncPrimitiveData* syncPrimitiveData);


	//Threads management
	ESTATE	_state{ CREATED };
	std::atomic_bool	_threadsKeepRunning{ true };
	std::atomic_bool	_threadsEmitError{ false };
	std::atomic_bool	_threadsBarrier{ true };
	std::atomic<Uint>	_threadsCountSignal{ 0u };

	std::vector<std::thread*>	_threads{};
	std::vector<FiberThreadData>	_fiberThreadData{};


	//
	//FiberFuncData	_fibersFuncData{};
	FiberPool _fiberPool{};

	//Spinlock	_syncPrimitiveDataLock{};
	//Spinlock	_taskSyncPrimitiveDataLock{};

	Uint	_fiberCount{};
};

}