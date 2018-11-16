#include "scheduler/fiber/FiberEntryPoint.h"

#include "scheduler/fiber/FiberFuncData.h"


void __stdcall 
FiberEntryPoint(
	void * data)
{
	using namespace chrone::multithreading::scheduler;

	FiberFuncData*	fiberFuncData = static_cast<FiberFuncData*>(data);
	auto&	keepRunning{ *(fiberFuncData->keepRunning) };
	auto&	schedulerProxy{ fiberFuncData->schedulerProxy };

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
