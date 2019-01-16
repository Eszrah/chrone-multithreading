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

	Wait for jobs to be finished -> need access to scheduler

	{
		std::lock_guard<std::mutex> lk{ fence->mutex };
		fence->fenceSignaled = 1u;
	}
	fence->conditionVariable.notify_all();
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
	Semaphore*	semaphores{ scheduler.semaphores };
	
	const HSemaphore hSemaphore{ fences[hFence.handle].hSemaphore };
	Semaphore& semaphore{ semaphores[hSemaphore.handle] };

	semaphore.dependentCounter.fetch_add(count, std::memory_order_relaxed);

	//We want to make sure the write can't be reordered after the push
	std::atomic_thread_fence(std::memory_order_release);

	return TaskPoolFunction::PushTasks(
		scheduler.taskPool, count, tasks, hSemaphore.handle);
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