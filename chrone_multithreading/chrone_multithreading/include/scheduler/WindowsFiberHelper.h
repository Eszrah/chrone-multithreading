#pragma once

using FiberEntryPointFunctor = void(__stdcall*)(void*);

namespace chrone::multithreading::fiberScheduler
{

struct WindowsFiberHelper
{
	static void*	ConvertThreadToFiber(void* fiberData);
	static bool	ConvertFiberToThread();

	static void*	AllocateHFiber(unsigned int stackSize, FiberEntryPointFunctor functor, void* fiberData);
	static void	DeallocateFiber(void* hFiber);

	static void	SwitchToFiber(void* hFiber);

	static void*	GetCurrentFiber();
	static void*	GetCurrentFiberData();

	template<class T>
	static T* GetCurrentFiberData() 
		{ return static_cast<T*>(GetCurrentFiberData()); }
};

}
