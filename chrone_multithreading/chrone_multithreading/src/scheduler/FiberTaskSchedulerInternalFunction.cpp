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
	TaskPoolFunction::PushTasks(
		scheduler.taskPool, count, tasks, hSemaphore.handle);
	return true;
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