#pragma once

#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;

struct WorkerThreadFuncData 
{
	Uint	threadIndex{};
	FiberTaskSchedulerData*	scheduler{ nullptr };
};

struct WorkerThreadFunction 
{
	static void	EntryPoint(WorkerThreadFuncData funcData);

	static bool	_Initialize(WorkerThreadFuncData funcData);
	static bool	_Shutdown(WorkerThreadFuncData funcData);
};
}