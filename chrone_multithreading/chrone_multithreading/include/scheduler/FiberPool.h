#pragma once

#include <vector>

#include "Fiber.h"
#include "NativeType.h"
#include "Spinlock.h"

namespace chrone::multithreading::fiberScheduler
{

struct FiberPool
{
	using SyncPrimitive = Spinlock;

	SyncPrimitive	freeFibersLock{};
	std::vector<Fiber*>	freeFibers{};
};

}