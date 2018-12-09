#pragma once

#include "NativeType.h"

namespace std
{
	template<class T>
	struct atomic;
}

namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;
struct ThreadsData;
struct TaskDecl;
struct Fence;

using HFence = void*;
using HSemaphore = void*;

struct FiberTaskSchedulerFunction
{

	static bool	Initialize(FiberTaskSchedulerData& scheduler,
		const Uint32 threadCount, const Uint32 fiberCount, const Uint32 fenceMaxCount);

	static bool	Shutdown(FiberTaskSchedulerData& scheduler);

	static HFence	AllocateFence(FiberTaskSchedulerData& scheduler);
	static void		FreeFence(FiberTaskSchedulerData& scheduler, HFence& fence);

	static bool	PushTasks(FiberTaskSchedulerData& scheduler, Uint32 count, const TaskDecl* tasksDecl, HFence hFence);
	static bool	WaitFence(FiberTaskSchedulerData& scheduler, HFence& hFence);

	static Fence*	_AllocateFence(FiberTaskSchedulerData& scheduler);
	static void		_FreeFence(FiberTaskSchedulerData& scheduler, Fence* fence);

	static void	_WaitAnddResetCounter(std::atomic<Uint>& counter, Uint count);
	static void _JoinThreads(ThreadsData& threadsData);
	static void _Clear(FiberTaskSchedulerData& scheduler);
};

}