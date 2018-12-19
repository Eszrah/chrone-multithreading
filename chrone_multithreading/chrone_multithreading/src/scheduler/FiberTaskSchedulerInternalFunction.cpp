#include "scheduler/FiberTaskSchedulerInternalFunction.h"

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/FiberFunction.h"
#include "scheduler/HSemaphore.h"


namespace chrone::multithreading::scheduler
{

bool 
FiberTaskSchedulerInternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler, 
	Uint32 count, 
	TaskDecl* tasks, 
	HSemaphore hSemaphore)
{
	TaskDependency	dependency{};
	Semaphore*	semaphore{ static_cast<Semaphore*>(hSemaphore.data) };

	if (semaphore)
	{
		semaphore->dependentFiber = nullptr;
		dependency.dependentFiber = &semaphore->dependentFiber;
		dependency.dependentCounter = &semaphore->dependentCounter;
		dependency.fence = nullptr;
	}

	TaskPoolFunction::PushTasks(scheduler.taskPool, count, tasks, dependency);

	return true;
}


bool 
FiberTaskSchedulerInternalFunction::WaitSemaphore(
	FiberTaskSchedulerData& scheduler, 
	HSemaphore& hSemaphore)
{
	FiberData*	fiberData{ FiberFunction::GetFiberData() };
	Semaphore*	semaphore{ static_cast<Semaphore*>(hSemaphore.data) };

	semaphore->dependentFiber.store(fiberData->fiber, std::memory_order_relaxed);
	semaphore->dependentCounter.fetch_add(1, std::memory_order_relaxed);


	return true;
}

}