#pragma once

#include <vector>

#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;
struct FiberPool;
struct Fiber;

struct WorkerThreadFuncData 
{
	Uint	threadIndex{};
	FiberTaskSchedulerData*	scheduler{ nullptr };
};

struct WorkerThreadFunction 
{
	static void	EntryPoint(WorkerThreadFuncData funcData);

	static bool	_Initialize(WorkerThreadFuncData funcData);
	static bool	_Shutdown();
	static Fiber*	_GetFreeNativeFiber(FiberPool& fiberPool, const std::vector<Fiber>& threadFibers);
};
}