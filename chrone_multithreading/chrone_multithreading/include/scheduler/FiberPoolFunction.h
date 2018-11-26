#pragma once

#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

struct FiberPool;
struct Fiber;

struct FiberPoolFunction 
{
	static void	Clear(FiberPool& pool);
	static void	Reserve(FiberPool& pool, Uint fiberCount);
	static void	PushFreeFiber(FiberPool& pool, Fiber* fiber);
	static bool	TryPopReadyFiber(FiberPool& pool, Fiber*& fiber);
};

}