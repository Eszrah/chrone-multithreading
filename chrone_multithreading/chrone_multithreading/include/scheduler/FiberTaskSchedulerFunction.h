#pragma once

#include "NativeType.h"
#include "HFence.h"
#include "HSemaphore.h"

namespace std
{
	template<class T>
	struct atomic;
}

namespace chrone::multithreading::fiberScheduler
{

struct TaskSchedulerData;
struct ThreadsData;
struct TaskBuffer;
struct Fence;
struct Semaphore;


struct TaskSchedulerFunction
{

	static bool	Initialize(TaskSchedulerData& scheduler,
		const Uint16 threadCount, const Uint16 fiberCount, const Uint32 maxTaskCountPowerOfTwo, const Uint16 fenceMaxCount, const Uint16 semaphroeMaxCount);

	static bool	Shutdown(TaskSchedulerData& scheduler);

	static HFence	AllocateFence(TaskSchedulerData& scheduler);
	static void		DeallocateFence(TaskSchedulerData& scheduler, HFence fence);

	static HSemaphore	AllocateSemaphore(TaskSchedulerData& scheduler);
	static void		DeallocateSemaphore(TaskSchedulerData& scheduler, HSemaphore semaphore);

	static HFence	_AllocateFence(TaskSchedulerData& scheduler);
	static void		_DeallocateFence(TaskSchedulerData& scheduler, HFence fence);

	static HSemaphore	_AllocateSemaphore(TaskSchedulerData& scheduler);
	static void		_DeallocateSemaphore(TaskSchedulerData& scheduler, HSemaphore semaphore);

	static void	_WaitAnddResetCounter(std::atomic<Uint>& counter, Uint count);
	static void _JoinThreads(ThreadsData& threadsData);
	static void _Clear(TaskSchedulerData& scheduler);
};

}