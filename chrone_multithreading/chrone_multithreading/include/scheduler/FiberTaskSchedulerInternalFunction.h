#pragma once

#include "NativeType.h"


namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;
struct TaskDecl;
struct HSemaphore;

struct FiberTaskSchedulerInternalFunction
{
	static bool	SubmitTasks(FiberTaskSchedulerData& scheduler, Uint32 count, TaskDecl* tasks, HSemaphore hSemaphore);
	static bool	SubmitTasks(FiberTaskSchedulerData& scheduler, Uint32 count, TaskDecl* tasks);
	static bool	WaitSemaphore(FiberTaskSchedulerData& scheduler, HSemaphore& hSemaphore);
};

}