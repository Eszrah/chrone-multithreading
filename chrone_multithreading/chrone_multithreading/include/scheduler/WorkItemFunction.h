#pragma once

namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;

struct WorkItemFunction
{
	static void	MainLoop(FiberTaskSchedulerData& scheduler);
};

}