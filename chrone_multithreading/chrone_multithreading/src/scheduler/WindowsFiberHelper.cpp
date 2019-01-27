#include "scheduler/WindowsFiberHelper.h"

#include <windows.h>


namespace chrone::multithreading::fiberScheduler
{

void* 
WindowsFiberHelper::ConvertThreadToFiber(
	void* fiberData)
{
	return ::ConvertThreadToFiber(fiberData);
}


bool 
WindowsFiberHelper::ConvertFiberToThread()
{
	return ::ConvertFiberToThread();
}


void* 
WindowsFiberHelper::AllocateHFiber(
	unsigned int stackSize, 
	FiberEntryPointFunctor functor, 
	void* fiberData)
{
	return ::CreateFiber(stackSize, functor, fiberData);
}


void 
WindowsFiberHelper::DeallocateFiber(
	void * hFiber)
{
	::DeleteFiber(hFiber);
}


void 
WindowsFiberHelper::SwitchToFiber(
	void * hFiber)
{
	::SwitchToFiber(hFiber);
}


void* 
WindowsFiberHelper::GetCurrentFiber()
{
	return ::GetCurrentFiber();
}

void * WindowsFiberHelper::GetCurrentFiberData()
{
	return ::GetFiberData();
}

}