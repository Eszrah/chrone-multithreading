#pragma once

#include <atomic>
#include <thread>

#include "FiberPool.h"
#include "ThreadFiberData.h"
#include "FiberData.h"

namespace chrone::multithreading::scheduler
{

struct ThreadsData
{
	std::atomic_bool	threadsKeepRunning{ true };
	std::atomic_bool	threadsEmitError{ false };
	std::atomic_bool	threadsBarrier{ true };
	std::atomic<Uint>	threadsCountSignal{ 0u };

	std::vector<std::thread*>	threads{};
};

struct FiberTaskSchedulerData
{
	ThreadsData	threadsData{};
	std::vector<ThreadFiberData>	threadFibersData{};

	std::vector<FiberData>	fibersData{};
	std::vector<Fiber>	fibers{};
	std::vector<Fiber>	threadsFibers{};

	FiberPool	fiberPool;
};

}
