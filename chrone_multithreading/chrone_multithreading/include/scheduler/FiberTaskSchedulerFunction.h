#pragma once

#include "NativeType.h"
#include "HFence.h"
#include "HSemaphore.h"

namespace std
{
	template<class T>
	struct atomic;
}

namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;
struct ThreadsData;
struct TaskBuffer;
struct Fence;
struct Semaphore;


struct FiberTaskSchedulerFunction
{

	static bool	Initialize(FiberTaskSchedulerData& scheduler,
		const Uint16 threadCount, const Uint16 fiberCount, const Uint32 maxTaskCountPowerOfTwo, const Uint16 fenceMaxCount, const Uint16 semaphroeMaxCount);

	static bool	Shutdown(FiberTaskSchedulerData& scheduler);

	static HFence	AllocateFence(FiberTaskSchedulerData& scheduler);
	static void		FreeFence(FiberTaskSchedulerData& scheduler, HFence fence);

	static HSemaphore	AllocateSemaphore(FiberTaskSchedulerData& scheduler);
	static void		FreeSemaphre(FiberTaskSchedulerData& scheduler, HSemaphore semaphore);

	static HFence	_AllocateFence(FiberTaskSchedulerData& scheduler);
	static void		_FreeFence(FiberTaskSchedulerData& scheduler, HFence fence);

	static HSemaphore	_AllocateSemaphore(FiberTaskSchedulerData& scheduler);
	static void		_FreeSemaphore(FiberTaskSchedulerData& scheduler, HSemaphore semaphore);

	static void	_WaitAnddResetCounter(std::atomic<Uint>& counter, Uint count);
	static void _JoinThreads(ThreadsData& threadsData);
	static void _Clear(FiberTaskSchedulerData& scheduler);
};

}