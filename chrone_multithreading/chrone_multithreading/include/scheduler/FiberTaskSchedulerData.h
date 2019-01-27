#pragma once

#include <atomic>
#include <thread>

#include "FiberPool.h"
#include "ThreadFiberData.h"
#include "FiberData.h"
#include "TaskPool.h"
#include "HSemaphore.h"

namespace chrone::multithreading::fiberScheduler
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

	//This lock is here in case of false sharing would harm memory coherency
	Spinlock	shutdownStateLock{};
	std::vector<bool>	threadsShutdownState{};
};

struct TaskSchedulerData
{
	constexpr static Uint32	defaultHSyncPrimitive{ 0u };
	constexpr static Uint32	invalidHSyncPrimitive{ 0xFFFFFFFF };

	ThreadsData	threadsData{};
	std::vector<ThreadFiberData>	threadFibersData{};

	Spinlock	fenceLock{};
	std::vector<Uint32>	freeFencesIndices{};
	Fence*	fences{};

	Spinlock	semaphoreLock{};
	Uint32	semaphoreMaxCount{};
	std::vector<Uint32>	freeSemaphoresIndices{};
	Semaphore*	semaphores{};
	 
	std::vector<FiberData>	fibersData{};
	std::vector<Fiber>	fibers{};
	std::vector<Fiber>	threadsFibers{};

	TaskPool	taskPool{};
	Uint32		nativeFiberCount{};
	FiberPool	fiberPool{};
	std::atomic<Uint>	defaultAtomicCounter{};
};

}
