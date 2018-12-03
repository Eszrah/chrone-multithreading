#include "scheduler/FiberFunction.h"

#include <atomic>
#include <cassert>

#include "scheduler/FiberPoolFunction.h"

#include "scheduler/WindowsFiberHelper.h"
#include "scheduler/ThreadFiberData.h"
#include "scheduler/FiberData.h"
#include "scheduler/Fiber.h"

namespace chrone::multithreading::scheduler
{

FiberData* 
FiberFunction::GetFiberData()
{
	return WindowsFiberHelper::GetCurrentFiberData<FiberData>();
}


void 
FiberFunction::SwitchToFiber(
	FiberPool& fiberPool,
	ThreadFiberData& threadFiberData,
	Fiber* newFiber)
{
	//Setup the new fiber data (to allow it to properly retrieve its thread once it restart)
	FiberData*	fiberData{ threadFiberData.currentFiber->fiberData };
	FiberData*	newFiberData{ newFiber->fiberData };

	assert(!threadFiberData.previousFiber);
	newFiberData->threadIndex = fiberData->threadIndex;

	threadFiberData.previousFiber = threadFiberData.currentFiber;
	threadFiberData.currentFiber = newFiber;
	WindowsFiberHelper::SwitchToFiber(newFiber->fiberHandle);
}


bool
FiberFunction::ExecuteFiberTask(
	Task& task)
{
	TaskDecl&	taskDecl{ task.decl };

	taskDecl.functor(taskDecl.data);

	if (task.dependencyCoouter)
	{
		auto const	lastDependencyCount{ task.dependencyCoouter->fetch_sub(
			1u, std::memory_order_release) };

		return task.dependencyFiber && task.dependencyFiber->load(
			std::memory_order_relaxed) && (1u == lastDependencyCount);
	}

	return false;
}

}