#pragma once

#include "NativeType.h"
#include <atomic>

namespace chrone::multithreading::scheduler
{

struct Fiber;

struct Fence
{
	std::atomic<Fiber*>	dependantFiber{};
	std::atomic<Uint>	counter{};
};

}