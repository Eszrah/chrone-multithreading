#include "scheduler/FiberPoolFunction.h"
#include "scheduler/FiberPool.h"

namespace chrone::multithreading::scheduler
{

void 
FiberPoolFunction::Clear(
	FiberPool& pool)
{
	pool.freeFibers.clear();
	pool.readyFibers.clear();
}


void
FiberPoolFunction::Reserve(
	FiberPool& pool, 
	Uint fiberCount)
{
	pool.freeFibers.reserve(fiberCount);
	pool.readyFibers.reserve(fiberCount);
}


void 
FiberPoolFunction::PushFreeFiber(
	FiberPool& pool, 
	Fiber* fiber)
{
}


bool 
FiberPoolFunction::TryPopReadyFiber(
	FiberPool& pool, 
	Fiber*& fiber)
{
	return false;
}

}