#include "scheduler/FiberPoolFunction.h"

#include <cassert>

#include "scheduler/FiberPool.h"

#include "std_extension/SpinlockStdExt.h"

namespace chrone::multithreading::scheduler
{

void 
FiberPoolFunction::Clear(
	FiberPool& pool)
{
	pool.freeFibers.clear();
}


void
FiberPoolFunction::Reserve(
	FiberPool& pool, 
	Uint fiberCount)
{
	pool.freeFibers.reserve(fiberCount);
}


void 
FiberPoolFunction::PushFreeFiber(
	FiberPool& pool, 
	Fiber* fiber)
{
	LockGuardSpinLock	lock{ pool.freeFibersLock };
	pool.freeFibers.push_back(fiber);
}


void 
FiberPoolFunction::PushFreeFibers_NotConcurrent(
	FiberPool& pool, 
	const Uint count, 
	Fiber* fiber)
{
	std::vector<Fiber*>&	freeFibers{ pool.freeFibers };
	const auto	oldFreeFibersCount{ freeFibers.size() };

	freeFibers.resize(oldFreeFibersCount + count);

	for (auto index{ oldFreeFibersCount }; 
		index < oldFreeFibersCount + count; ++index, ++fiber)
	{
		freeFibers[index] = fiber;
	}
}


Fiber* 
FiberPoolFunction::PopFreeFiber(
	FiberPool& pool)
{
	LockGuardSpinLock	lock{ pool.freeFibersLock };
	std::vector<Fiber*>&	freeFibers{ pool.freeFibers };
	assert(!freeFibers.empty());
	Fiber*	fiber{ freeFibers.back() };

	freeFibers.pop_back();
	return fiber;
}

}