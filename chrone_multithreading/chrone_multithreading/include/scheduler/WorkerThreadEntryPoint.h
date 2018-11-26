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


void WorkerThreadEntryPoint(WorkerThreadFuncData funcData);

}