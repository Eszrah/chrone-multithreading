#pragma once

namespace chrone::multithreading::fiberScheduler
{

struct TaskSchedulerData;

struct FiberMainLoop
{
	static void	MainLoop(TaskSchedulerData& scheduler);
};

}