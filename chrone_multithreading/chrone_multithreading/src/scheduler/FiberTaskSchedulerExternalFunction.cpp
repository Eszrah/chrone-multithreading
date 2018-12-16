#include "scheduler/FiberTaskSchedulerExternalFunction.h"

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/HFence.h"


namespace chrone::multithreading::scheduler
{


bool
FiberTaskSchedulerExternalFunction::SubmitTasks(
	FiberTaskSchedulerData& scheduler,
	Uint32 count, 
	TaskDecl* tasks,
	HFence hFence)
{
	TaskDependency	dependency{};
	Fence*	fence{ static_cast<Fence*>(hFence.data) };

	if (fence)
	{
		dependency.dependentCounter = &fence->dependantCounter;
		dependency.fence = &fence->conditionVariable;
		dependency.dependentFiber = nullptr;
	}

	TaskPoolFunction::PushTasks(scheduler.taskPool, count, tasks, dependency);
	return true;
}


bool 
FiberTaskSchedulerExternalFunction::WaitFence(
	FiberTaskSchedulerData& scheduler,
	HFence& hFence)
{
	Fence*	fence{ static_cast<Fence*>(hFence.data) };
	std::unique_lock	lock{ fence->mutex };

	fence->conditionVariable.wait(lock);

	return true;
}

}