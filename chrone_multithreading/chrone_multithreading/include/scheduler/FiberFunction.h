#pragma once

namespace chrone::multithreading::scheduler
{

struct FiberData;
struct ThreadFiberData;
struct FiberPool;
struct Fiber;
struct Task;

struct FiberFunction
{
	static FiberData*	GetFiberData();
	static void	SwitchToFiber(FiberPool& fiberPool, ThreadFiberData& threadFiberData, Fiber* newFiber);

};

}