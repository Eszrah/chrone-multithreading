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

struct FiberPoolSchedulerFunction 
{

	static bool	Initialize(FiberTaskSchedulerData& scheduler,
		Uint threadCount, Uint fiberCount);

	static bool	Shutdown(FiberTaskSchedulerData& scheduler);


	static void	_WaitAnddResetCounter(std::atomic<Uint>& counter, Uint count);
	static void _JoinAndDeleteThreads(ThreadsData& threadsData);
};

}