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

struct HSemaphore;

struct FiberTaskSchedulerFunction
{

	static bool	Initialize(FiberTaskSchedulerData& scheduler,
		const Uint32 threadCount, const Uint32 fiberCount, const Uint32 maxTaskCountPowerOfTwo, const Uint32 fenceMaxCount, const Uint32 semaphroeMaxCount);

	static bool	Shutdown(FiberTaskSchedulerData& scheduler);

	static HFence	AllocateFence(FiberTaskSchedulerData& scheduler);
	static void		FreeFence(FiberTaskSchedulerData& scheduler, HFence& fence);

	static HSemaphore	AllocateSemaphore(FiberTaskSchedulerData& scheduler);
	static void		FreeSemaphre(FiberTaskSchedulerData& scheduler, HSemaphore& semaphore);

	static Fence*	_AllocateFence(FiberTaskSchedulerData& scheduler);
	static void		_FreeFence(FiberTaskSchedulerData& scheduler, Fence* fence);

	static Semaphore*	_AllocateSemaphore(FiberTaskSchedulerData& scheduler);
	static void		_FreeSemaphore(FiberTaskSchedulerData& scheduler, Semaphore* semaphore);

	static void	_WaitAnddResetCounter(std::atomic<Uint>& counter, Uint count);
	static void _JoinThreads(ThreadsData& threadsData);
	static void _Clear(FiberTaskSchedulerData& scheduler);
};

}