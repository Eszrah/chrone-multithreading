#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberTaskSchedulerFunction.h"

#include <iostream>
#include <chrono>
#include <thread>

using namespace chrone::multithreading::scheduler;

int main()
{
	FiberTaskSchedulerData	scheduler;
	const Uint	threadCount = 8;
	const Uint	fiberCount = 100;

	FiberTaskSchedulerFunction::Initialize(scheduler, threadCount, fiberCount);

	while (true)
	{

	}

	FiberTaskSchedulerFunction::Shutdown(scheduler);

	return 0;
}
