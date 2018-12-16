#pragma once

#include "NativeType.h"


namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;
struct TaskDecl;
struct HFence;

struct FiberTaskSchedulerExternalFunction 
{
	static bool	SubmitTasks(FiberTaskSchedulerData& scheduler, Uint32 count, TaskDecl* tasks, HFence hFence);
	static bool	WaitFence(FiberTaskSchedulerData& scheduler, HFence& hFence);
};

}