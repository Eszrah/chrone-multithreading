#include "scheduler/FiberTaskSchedulerExternalFunction.h"

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/TaskDecl.h"
#include "scheduler/SyncPrimitive.h"
#include "scheduler/HFence.h"

#include "AssertMacro.h"

namespace chrone::multithreading::scheduler
{

void 
WaitFenceJob(
	void* data)
{
	Fence*	fence{ static_cast<Fence*>(data) };

	//Wait for jobs to be finished -> need access to scheduler

	{
		std::lock_guard<std::mutex> lk{ fence->mutex };
		fence->fenceSignaled = 1u;
	}
	fence->conditionVariable.notify_all();
}


bool
FiberTaskSchedulerExternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler,
	Uint32 count, 
	const TaskDecl* tasks,
	HFence hFence)
{
	Fence*	fences{ scheduler.fences };
	const HSemaphore hSemaphore{ fences[hFence.handle].hSemaphore };

	return TaskPoolFunction::PushTasks(
		scheduler.taskPool, count, tasks, hSemaphore.handle);
	return true;
}


bool 
FiberTaskSchedulerExternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler, 
	Uint32 count, 
	const TaskDecl* tasks)
{
	return TaskPoolFunction::PushTasks( scheduler.taskPool, count, tasks, 
		{FiberTaskSchedulerData::defaultHSyncPrimitive});
}


bool 
FiberTaskSchedulerExternalFunction::WaitFence(
	FiberTaskSchedulerData& scheduler,
	HFence& hFence)
{
	Fence*	fence{ &scheduler.fences[hFence.handle] };
	const TaskDecl	waitTaskDecl{ WaitFenceJob, fence };

	TaskPoolFunction::PushTasks(scheduler.taskPool, 1, &waitTaskDecl,
		{ FiberTaskSchedulerData::defaultHSyncPrimitive });

	std::unique_lock<std::mutex>	lock{ fence->mutex };

	CHR_ASSERT(!fence->fenceSignaled);
	fence->conditionVariable.wait(lock, 
		[fenceSignaled = std::ref(fence->fenceSignaled)]
			{ return fenceSignaled; });

	return true;
}

}