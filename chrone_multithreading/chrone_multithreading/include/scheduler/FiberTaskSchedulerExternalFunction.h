#pragma once

#include "NativeType.h"


namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;
struct TaskDecl;
struct HFence;

struct FiberTaskSchedulerExternalFunction 
{
	static bool	SubmitTasks(FiberTaskSchedulerData& scheduler, const Uint32 count, const TaskDecl* tasks, HFence hFence);
	static bool	SubmitTasks(FiberTaskSchedulerData& scheduler, const Uint32 count, const TaskDecl* tasks);
	static bool	WaitFence(FiberTaskSchedulerData& scheduler, HFence& hFence);
};

}