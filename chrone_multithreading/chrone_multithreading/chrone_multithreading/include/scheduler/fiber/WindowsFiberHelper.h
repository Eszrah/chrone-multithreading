#pragma once

using FiberEntryPointFunctor = void(__stdcall*)(void*);

namespace chrone::multithreading::scheduler::fiber
{

struct WindowsFiberHelper
{
	static void*	ConvertThreadToFiber(void* fiberData);
	static bool	ConvertFiberToThread();

	static void*	AllocateHFiber(unsigned int stackSize, FiberEntryPointFunctor functor, void* fiberData);
	static void	DeallocateFiber(void* hFiber);

	static void	SwitchToFiber(void* hFiber);

	static void*	GetCurrentFiber();
};

}
