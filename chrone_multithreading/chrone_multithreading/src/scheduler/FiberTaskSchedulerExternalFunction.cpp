#include "scheduler/FiberTaskSchedulerExternalFunction.h"

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberTaskSchedulerInternalFunction.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/TaskDecl.h"
#include "scheduler/SyncPrimitive.h"
#include "scheduler/HFence.h"

#include "AssertMacro.h"

namespace chrone::multithreading::fiberScheduler
{

struct WaitFenceJobData
{
	TaskSchedulerData*	scheduler{ nullptr };
	Fence*	fence{ nullptr };
};


void 
WaitFenceJob(
	void* data)
{
	WaitFenceJobData*	jobData{ static_cast<WaitFenceJobData*>(data) };
	TaskSchedulerData& scheduler{ *jobData->scheduler };
	Fence& fence{ *jobData->fence };
	const HSemaphore hSemaphore{ fence.hSemaphore };

	TaskSchedulerInternalFunction::WaitSemaphore(scheduler, hSemaphore);

	{
		std::lock_guard<std::mutex> lk{ fence.mutex };
		fence.fenceSignaled = 1u;
	}
	fence.conditionVariable.notify_all();
}


bool
TaskSchedulerExternalFunction::SubmitTasks(
	TaskSchedulerData& scheduler,
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

	assert(hFence.handle != TaskSchedulerData::defaultHSyncPrimitive &&
		hFence.handle != TaskSchedulerData::invalidHSyncPrimitive);
	
	return TaskSchedulerInternalFunction::SubmitTasks(scheduler, 
		count, tasks, hSemaphore);
}


bool 
TaskSchedulerExternalFunction::SubmitTasks(
	TaskSchedulerData& scheduler, 
	const Uint32 count, 
	const TaskDecl* tasks)
{
	return SubmitTasks(scheduler, count, tasks, { scheduler.defaultHSyncPrimitive });
}


bool 
TaskSchedulerExternalFunction::WaitFence(
	TaskSchedulerData& scheduler,
	HFence& hFence)
{
	Fence*	fence{ &scheduler.fences[hFence.handle] };
	WaitFenceJobData	waitFenceJobData{ &scheduler, fence };
	const TaskDecl	waitTaskDecl{ WaitFenceJob, &waitFenceJobData };

	assert(hFence.handle != TaskSchedulerData::defaultHSyncPrimitive &&
		hFence.handle != TaskSchedulerData::invalidHSyncPrimitive);

	TaskPoolFunction::PushTasks(scheduler.taskPool, 1, &waitTaskDecl,
		{ TaskSchedulerData::defaultHSyncPrimitive });

	std::unique_lock<std::mutex>	lock{ fence->mutex };
	fence->conditionVariable.wait(lock, 
		[fenceSignaled = std::ref(fence->fenceSignaled)]
			{ return fenceSignaled; });

	return true;
}

}