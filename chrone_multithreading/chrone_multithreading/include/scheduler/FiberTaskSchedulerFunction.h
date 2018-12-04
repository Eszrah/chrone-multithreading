#pragma once

#include "NativeType.h"
#include "FenceHandle.h"

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

struct FiberTaskSchedulerFunction
{

	static bool	Initialize(FiberTaskSchedulerData& scheduler,
		const Uint threadCount, const Uint fiberCount);

	static bool	Shutdown(FiberTaskSchedulerData& scheduler);

	static bool AllocateFence(Uint32 count, HFence* hFences);
	//static bool	PushTasks(Uint32 count, const TaskDecl* tasksDecl, );

	static void	_WaitAnddResetCounter(std::atomic<Uint>& counter, Uint count);
	static void _JoinThreads(ThreadsData& threadsData);
	static void _Clear(FiberTaskSchedulerData& scheduler);
};

}