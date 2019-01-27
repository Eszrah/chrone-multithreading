#pragma once

#include "NativeType.h"

namespace chrone::multithreading::fiberScheduler
{

struct FiberPool;
struct Fiber;

struct FiberPoolFunction
{
	static void	Clear(FiberPool& pool);
	static void	Reserve(FiberPool& pool, Uint fiberCount);
	static void	PushFreeFiber(FiberPool& pool, Fiber* fiber);
	static void	PushFreeFibers_NotConcurrent(FiberPool& pool, const Uint count, Fiber* fiber);
	static Fiber* PopFreeFiber(FiberPool& pool);
};

}