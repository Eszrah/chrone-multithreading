#include "WindowsFiberHelper.h"

#include <windows.h>

namespace chrone::multithreading::scheduler::fiber
{

void* WindowsFiberHelper::ConvertThreadToFiber(void* fiberData)
{
	return ::ConvertThreadToFiber(fiberData);
}
void* WindowsFiberHelper::AllocateHFiber(Uint32 stackSize, FiberEntryPointFunctor functor, void* fiberData)
{
	return ::CreateFiber(stackSize, functor, fiberData);
}

void WindowsFiberHelper::DeallocateFiber(void * hFiber)
{
	::DeleteFiber(hFiber);
}


}