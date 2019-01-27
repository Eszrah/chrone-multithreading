#pragma once

#include "NativeType.h"

namespace chrone::multithreading::fiberScheduler
{

struct TaskSchedulerData;

struct FiberData
{
	FiberData() = default;

	FiberData(const Uint8 threadIndex, Fiber* fiber, TaskSchedulerData* scheduler):
		threadIndex{ threadIndex },
		fiber{ fiber },
		scheduler{ scheduler }
	{}

	Uint8	threadIndex{ 0xFF };
	Fiber*	fiber{ nullptr };
	TaskSchedulerData*	scheduler{ nullptr };
};

}