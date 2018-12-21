#include "scheduler/FiberTaskSchedulerInternalFunction.h"

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/FiberFunction.h"
#include "scheduler/HSemaphore.h"
#include "scheduler/SyncPrimitive.h"


namespace chrone::multithreading::scheduler
{

bool 
FiberTaskSchedulerInternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler, 
	Uint32 count, 
	TaskDecl* tasks, 
	HSemaphore hSemaphore)
{
	Semaphore& semaphore{ scheduler.semaphores[hSemaphore.handle] };

	semaphore.dependentCounter.fetch_add(count, std::memory_order_relaxed);

	//We want to make sure the write can't be reordered after the push
	std::atomic_thread_fence(std::memory_order_release);

	return TaskPoolFunction::PushTasks(
		scheduler.taskPool, count, tasks, hSemaphore.handle);
}


bool 
FiberTaskSchedulerInternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler, 
	Uint32 count, 
	TaskDecl* tasks)
{
	return SubmitTasks(scheduler, count, tasks, { scheduler.defaultHSyncPrimitive });
}


bool 
FiberTaskSchedulerInternalFunction::WaitSemaphore(
	FiberTaskSchedulerData& scheduler, 
	HSemaphore& hSemaphore)
{
	//FiberData*	fiberData{ FiberFunction::GetFiberData() };
	//Semaphore*	semaphore{ static_cast<Semaphore*>(hSemaphore.data) };

	//semaphore->dependentFiber.store(fiberData->fiber, std::memory_order_relaxed);
	//semaphore->dependentCounter.fetch_add(1, std::memory_order_relaxed);


	return true;
}

}