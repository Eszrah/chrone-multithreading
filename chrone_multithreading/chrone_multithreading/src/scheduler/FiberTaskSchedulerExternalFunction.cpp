#include "scheduler/FiberTaskSchedulerExternalFunction.h"

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/HFence.h"
#include "scheduler/SyncPrimitive.h"


namespace chrone::multithreading::scheduler
{


bool
FiberTaskSchedulerExternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler,
	Uint32 count, 
	TaskDecl* tasks,
	HFence hFence)
{
	Fence*	fences{ scheduler.fences };
	const HSemaphore hSemaphore{ 
		hFence.handle == FiberTaskSchedulerData::defaultHSyncPrimitive ?
		HSemaphore{ FiberTaskSchedulerData::defaultHSyncPrimitive } :
		fences[hFence.handle].hSemaphore };

	TaskPoolFunction::PushTasks(
		scheduler.taskPool, count, tasks, hSemaphore.handle);
	return true;
}


bool 
FiberTaskSchedulerExternalFunction::WaitFence(
	FiberTaskSchedulerData& scheduler,
	HFence& hFence)
{
	//Fence*	fence{ static_cast<Fence*>(hFence.data) };
	//std::unique_lock	lock{ fence->mutex };

	//fence->conditionVariable.wait(lock);

	return true;
}

}