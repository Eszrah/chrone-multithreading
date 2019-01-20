#pragma once

#include "NativeType.h"


namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;
struct TaskDecl;
struct HSemaphore;

struct FiberTaskSchedulerInternalFunction
{
	static bool	SubmitTasks(FiberTaskSchedulerData& scheduler, const Uint32 count, const TaskDecl* tasks, HSemaphore hSemaphore);
	static bool	SubmitTasks(FiberTaskSchedulerData& scheduler, const Uint32 count, const TaskDecl* tasks);
	static bool	WaitSemaphore(FiberTaskSchedulerData& scheduler, HSemaphore hSemaphore);
};

}
