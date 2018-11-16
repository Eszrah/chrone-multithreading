#pragma once

#include "NativeType.h"
#include "TaskDecl.h"
#include "TaskSyncPrimitive.h"

namespace chrone::multithreading::scheduler
{

class FiberTaskScheduler;

class FiberTaskSchedulerAgent
{
public:
	~FiberTaskSchedulerAgent() = default;

	bool	AllocateSyncPrimitive(TaskSyncPrimitive& taskSyncPrimitive);
	bool	DeallocateSyncPrimitive(TaskSyncPrimitive& taskSyncPrimitive);
	bool	PushTasks(Uint count, TaskDecl* tasks, TaskSyncPrimitive* syncPrim = nullptr);
	bool	Wait(TaskSyncPrimitive& syncPrim);
	bool	CheckFinished(TaskSyncPrimitive& syncPrim);

private:
	FiberTaskSchedulerAgent() = default;

	friend class FiberTaskScheduler;

	FiberTaskScheduler*	_scheduler{ nullptr };
};

}