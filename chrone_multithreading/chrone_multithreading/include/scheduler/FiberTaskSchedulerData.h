#pragma once

#include <atomic>
#include <thread>

#include "FiberPool.h"
#include "ThreadFiberData.h"
#include "FiberData.h"
#include "Fence.h"
#include "TaskPool.h"

namespace chrone::multithreading::scheduler
{

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
	ThreadsData	threadsData{};
	std::vector<ThreadFiberData>	threadFibersData{};

	Uint32	fenceMaxCount{};
	Uint32	fenceCount{};
	Spinlock	fenceLock{};
	std::vector<Uint32>	freeFencesIndices{};
	std::vector<Fence>	fibersData{};

	std::vector<FiberData>	fibersData{};
	std::vector<Fiber>	fibers{};
	std::vector<Fiber>	threadsFibers{};


	TaskPool	taskPool;
	FiberPool	fiberPool;
};

}
