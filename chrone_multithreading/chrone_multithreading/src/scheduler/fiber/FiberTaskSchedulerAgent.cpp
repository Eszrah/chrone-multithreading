#include "scheduler/fiber/FiberTaskSchedulerAgent.h"

#include "scheduler/fiber/FiberTaskScheduler.h"

namespace chrone::multithreading::scheduler
{

bool 
FiberTaskSchedulerAgent::AllocateSyncPrimitive(
	TaskSyncPrimitive& taskSyncPrimitive)
{
	return _scheduler->_AgentAllocateSyncPrimitive(taskSyncPrimitive);
}


bool 
FiberTaskSchedulerAgent::DeallocateSyncPrimitive(
	TaskSyncPrimitive& taskSyncPrimitive)
{
	return _scheduler->_AgentDeallocateSyncPrimitive(taskSyncPrimitive);
}


bool
FiberTaskSchedulerAgent::PushTasks(
	Uint count, 
	TaskDecl* tasks, 
	TaskSyncPrimitive* syncPrim)
{
	return _scheduler->_AgentPushTasks(count, tasks, syncPrim);
}


bool 
FiberTaskSchedulerAgent::Wait(
	TaskSyncPrimitive& syncPrim)
{
	return false;
}


bool 
FiberTaskSchedulerAgent::CheckFinished(
	TaskSyncPrimitive& syncPrim)
{
	return false;
}

}