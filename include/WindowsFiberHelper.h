#pragma once

#include "NativeType.h"

using FiberEntryPointFunctor = void(__stdcall*)(void*);

namespace chrone::multithreading::scheduler::fiber
{

struct WindowsFiberHelper
{
	static void*	ConvertThreadToFiber(void* fiberData);
	static void*	AllocateHFiber(Uint32 stackSize, FiberEntryPointFunctor functor, void* fiberData);
	static void		DeallocateFiber(void* hFiber);
};

}
