#pragma once

#include <atomic>
#include <thread>

#include "FiberPool.h"
#include "ThreadFiberData.h"
#include "FiberData.h"
#include "TaskPool.h"
#include "HSemaphore.h"

namespace chrone::multithreading::scheduler
{

struct Fence;
struct Semaphore;

struct ThreadsData
{
	std::atomic_bool	threadsKeepRunning{ true };
	std::atomic_bool	threadsEmitError{ false };
	std::atomic_bool	threadsBarrier{ true };
	std::atomic<Uint>	threadsCountSignal{ 0u };

	std::vector<std::thread*>	threads{};
	std::vector<bool>	threadsShutdownState{};
};

struct FiberTaskSchedulerData
{
	constexpr static Uint	defaultHSyncPrimitive{ 0u };

	ThreadsData	threadsData{};
	std::vector<ThreadFiberData>	threadFibersData{};

	Spinlock	fenceLock{};
	Uint32	fenceMaxCount{};
	std::vector<Uint32>	freeFencesIndices{};
	Fence*	fences{};

	Spinlock	semaphoreLock{};
	Uint32	semaphoreMaxCount{};
	Uint16	allocatedNativeSemaphore{};
	std::vector<Uint32>	freeSemaphoresIndices{};
	Semaphore*	semaphores{};
	 
	std::vector<FiberData>	fibersData{};
	std::vector<Fiber>	fibers{};
	std::vector<Fiber>	threadsFibers{};

	TaskPool	taskPool;
	FiberPool	fiberPool;
	std::atomic<Uint>	defaultAtomicCounter{};
};

}
