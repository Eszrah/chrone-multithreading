#include "scheduler/FiberTaskSchedulerInternalFunction.h"

#include <cassert>

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/FiberPoolFunction.h"
#include "scheduler/FiberFunction.h"
#include "scheduler/HSemaphore.h"
#include "scheduler/SyncPrimitive.h"


namespace chrone::multithreading::fiberScheduler
{

bool 
TaskSchedulerInternalFunction::SubmitTasks(
	TaskSchedulerData& scheduler, 
	const Uint32 count, 
	const TaskDecl* tasks, 
	HSemaphore hSemaphore)
{
	if (!count || !tasks)
	{
		return false;
	}

	Semaphore& semaphore{ scheduler.semaphores[hSemaphore.handle] };

	//There is no reason to set another memory order
	//We are sure that the store can't be reordered after the taskpool lock
	//So even if this store is performed after the task store in taskpool memory
	//it will be visible due to write release of the lock
	semaphore.dependentCounter.store( count + 1u, std::memory_order_relaxed );

	return TaskPoolFunction::PushTasks(
		scheduler.taskPool, count, tasks, hSemaphore.handle);
}


bool 
TaskSchedulerInternalFunction::SubmitTasks(
	TaskSchedulerData& scheduler, 
	const Uint32 count, 
	const TaskDecl* tasks)
{
	return SubmitTasks(scheduler, count, tasks, HSemaphore{ scheduler.defaultHSyncPrimitive });
}


bool 
TaskSchedulerInternalFunction::WaitSemaphore(
	TaskSchedulerData& scheduler, 
	HSemaphore hSemaphore)
{
	ThreadFiberData*	threadFibersData{ scheduler.threadFibersData.data() };
	FiberPool&			fiberPool{ scheduler.fiberPool };
	Fiber*				freeFiber{ FiberPoolFunction::PopFreeFiber(fiberPool) };
	const FiberData*	fiberData{ FiberFunction::GetFiberData() };
	Semaphore*			semaphore{ &scheduler.semaphores[hSemaphore.handle] };



	FiberFunction::SwitchToFiber(fiberPool, threadFibersData, 
		threadFibersData[fiberData->threadIndex], freeFiber, 
		semaphore, fiberData->fiber );
	return true;
}

}