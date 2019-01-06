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
	static const FiberData*	GetFiberData();
	static ThreadFiberData&	SwitchToFiber(FiberPool& fiberPool, ThreadFiberData* threadsFiberData, ThreadFiberData& fromThreadFiberData, Fiber* newFiber);
};

}