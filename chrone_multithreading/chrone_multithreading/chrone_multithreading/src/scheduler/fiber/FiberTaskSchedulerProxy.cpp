#include "scheduler/fiber/FiberTaskSchedulerProxy.h"

#include "scheduler/fiber/FiberTaskScheduler.h"

namespace chrone::multithreading::scheduler::fiber
{
void FiberTaskSchedulerProxy::DebugEntryCheck()
{
	_scheduler->_ProxyDebugEntryCheck();
}
void
FiberTaskSchedulerProxy::InitFiberFirstEntry()
{
	_scheduler->_ProxyInitFiberFirstEntry();
}


void 
FiberTaskSchedulerProxy::ExecuteTask()
{
	_scheduler->_ProxyExecuteTask();
}


void 
FiberTaskSchedulerProxy::SwitchBackToOriginalThread()
{
	_scheduler->_ProxySwitchBackToOriginalThread();
}


}