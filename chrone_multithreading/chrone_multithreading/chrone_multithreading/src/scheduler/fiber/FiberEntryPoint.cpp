#include "scheduler/fiber/FiberEntryPoint.h"

#include "scheduler/fiber/FiberData.h"


void __stdcall 
FiberEntryPoint(
	void * data)
{
	using namespace chrone::multithreading::scheduler::fiber;

	FiberData*	fiberData = static_cast<FiberData*>(data);
	auto&	keepRunning{ *(fiberData->keepRunning) };
	auto&	schedulerProxy{ fiberData->schedulerProxy };

	schedulerProxy.DebugEntryCheck();

	if (keepRunning)
	{
		schedulerProxy.InitFiberFirstEntry();
	}

	while (keepRunning)
	{
		schedulerProxy.ExecuteTask();
	}

	//GO BACK TO THE ORIGINAL THREAD FUNCTION TO END EXECUTION
	schedulerProxy.SwitchBackToOriginalThread();
}
