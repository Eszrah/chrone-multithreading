#include "scheduler/FiberTaskSchedulerInternalFunction.h"

#include <cassert>

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/FiberPoolFunction.h"
#include "scheduler/FiberFunction.h"
#include "scheduler/HSemaphore.h"
#include "scheduler/SyncPrimitive.h"


namespace chrone::multithreading::scheduler
{

bool 
FiberTaskSchedulerInternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler, 
	const Uint32 count, 
	const TaskDecl* tasks, 
	HSemaphore hSemaphore)
{
	if (!count || !tasks)
	{
		return false;
	}

	Semaphore& semaphore{ scheduler.semaphores[hSemaphore.handle] };

	semaphore.dependentCounter.fetch_add(count + 1u, std::memory_order_relaxed);

	//We want to make sure the write can't be reordered after the push
	make sure it is valid with regards to fence-atomic operation 
	std::atomic_thread_fence(std::memory_order_release);

	return TaskPoolFunction::PushTasks(
		scheduler.taskPool, count, tasks, hSemaphore.handle);
}


bool 
FiberTaskSchedulerInternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler, 
	const Uint32 count, 
	const TaskDecl* tasks)
{
	return SubmitTasks(scheduler, count, tasks, { scheduler.defaultHSyncPrimitive });
}


bool 
FiberTaskSchedulerInternalFunction::WaitSemaphore(
	FiberTaskSchedulerData& scheduler, 
	HSemaphore& hSemaphore)
{
	ThreadFiberData*	threadFibersData{ scheduler.threadFibersData.data() };
	FiberPool&	fiberPool{ scheduler.fiberPool };
	Fiber*	freeFiber{ FiberPoolFunction::PopFreeFiber(fiberPool) };
	const FiberData*	fiberData{ FiberFunction::GetFiberData() };
	Semaphore*	semaphore{ &scheduler.semaphores[hSemaphore.handle] };

	FiberFunction::SwitchToFiber(fiberPool, threadFibersData, 
		threadFibersData[fiberData->threadIndex], freeFiber, 
		semaphore, fiberData->fiber );
	return true;
}

}