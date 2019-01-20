#pragma once

namespace chrone::multithreading::scheduler
{

struct FiberTaskSchedulerData;

struct FiberMainLoop
{
	static void	MainLoop(FiberTaskSchedulerData& scheduler);
};

}