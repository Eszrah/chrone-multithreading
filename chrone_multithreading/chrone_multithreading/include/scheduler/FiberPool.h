#pragma once

#include <vector>

#include "Fiber.h"
#include "NativeType.h"
#include "Spinlock.h"

namespace chrone::multithreading::scheduler
{

struct FiberPool
{
	using SyncPrimitive = Spinlock;

	SyncPrimitive	freeFibersLock{};
	SyncPrimitive	readyFibersLock{};

	std::vector<Fiber*>	freeFibers{};
	std::vector<Fiber*>	readyFibers{};
};

}