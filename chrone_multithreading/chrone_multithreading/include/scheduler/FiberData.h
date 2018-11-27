#pragma once

#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;

struct FiberData
{
	FiberData() = default;

	FiberData(const Uint8 threadIndex, FiberTaskSchedulerData* scheduler):
		threadIndex{ threadIndex },
		scheduler{ scheduler }
	{}

	Uint8	threadIndex{ 0 };
	FiberTaskSchedulerData*	scheduler{ nullptr };
};

}