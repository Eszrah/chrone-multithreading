#include "scheduler/FiberTaskSchedulerExternalFunction.h"

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberTaskSchedulerInternalFunction.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/TaskDecl.h"
#include "scheduler/SyncPrimitive.h"
#include "scheduler/HFence.h"

#include "AssertMacro.h"

namespace chrone::multithreading::scheduler
{

struct WaitFenceJobData
{
	FiberTaskSchedulerData*	scheduler{ nullptr };
	Fence*	fence{ nullptr };
};


void 
WaitFenceJob(
	void* data)
{
	WaitFenceJobData*	jobData{ static_cast<WaitFenceJobData*>(data) };
	FiberTaskSchedulerData& scheduler{ *jobData->scheduler };
	Fence& fence{ *jobData->fence };
	const HSemaphore hSemaphore{ fence.hSemaphore };

	FiberTaskSchedulerInternalFunction::WaitSemaphore(scheduler, hSemaphore);

	{
		std::lock_guard<std::mutex> lk{ fence.mutex };
		fence.fenceSignaled = 1u;
	}
	fence.conditionVariable.notify_all();
}


bool
FiberTaskSchedulerExternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler,
	const Uint32 count, 
	const TaskDecl* tasks,
	HFence hFence)
{
	if (!count || !tasks)
	{
		return false;
	}

	Fence*	fences{ scheduler.fences };
	const HSemaphore hSemaphore{ fences[hFence.handle].hSemaphore };

	
	return FiberTaskSchedulerInternalFunction::SubmitTasks(scheduler, 
		count, tasks, hSemaphore);
}


bool 
FiberTaskSchedulerExternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler, 
	const Uint32 count, 
	const TaskDecl* tasks)
{
	return SubmitTasks(scheduler, count, tasks, { scheduler.defaultHSyncPrimitive });
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
	fence->conditionVariable.wait(lock, 
		[fenceSignaled = std::ref(fence->fenceSignaled)]
			{ return fenceSignaled; });

	return true;
}

}