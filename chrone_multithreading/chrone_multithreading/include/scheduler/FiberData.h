#pragma once

#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

struct FiberTaskScheduler;

struct FiberData
{
	Uint8	threadIndex{ 0xFFFFFFFF };
	FiberTaskScheduler*	scheduler{ nullptr };
};

}