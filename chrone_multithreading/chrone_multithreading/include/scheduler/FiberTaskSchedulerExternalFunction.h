#pragma once

#include "NativeType.h"


namespace chrone::multithreading::fiberScheduler
{

struct TaskSchedulerData;
struct TaskDecl;
struct HFence;

struct TaskSchedulerExternalFunction 
{
	static bool	SubmitTasks(TaskSchedulerData& scheduler, const Uint32 count, const TaskDecl* tasks, HFence hFence);
	static bool	SubmitTasks(TaskSchedulerData& scheduler, const Uint32 count, const TaskDecl* tasks);
	static bool	WaitFence(TaskSchedulerData& scheduler, HFence& hFence);
};

}