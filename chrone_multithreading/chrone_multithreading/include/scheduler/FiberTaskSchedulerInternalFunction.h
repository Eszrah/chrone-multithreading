#pragma once

#include "NativeType.h"


namespace chrone::multithreading::fiberScheduler
{

struct TaskSchedulerData;
struct TaskDecl;
struct HSemaphore;

struct TaskSchedulerInternalFunction
{
	static bool	SubmitTasks(TaskSchedulerData& scheduler, const Uint32 count, const TaskDecl* tasks, HSemaphore hSemaphore);
	static bool	SubmitTasks(TaskSchedulerData& scheduler, const Uint32 count, const TaskDecl* tasks);
	static bool	WaitSemaphore(TaskSchedulerData& scheduler, HSemaphore hSemaphore);
};

}
