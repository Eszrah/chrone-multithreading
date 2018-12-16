#pragma once

#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;

struct FiberData
{
	FiberData() = default;

	FiberData(const Uint8 threadIndex, Fiber* fiber, FiberTaskSchedulerData* scheduler):
		threadIndex{ threadIndex },
		fiber{ fiber },
		scheduler{ scheduler }
	{}

	Uint8	threadIndex{ 0xFF };
	Fiber*	fiber{ nullptr };
	FiberTaskSchedulerData*	scheduler{ nullptr };
};

}