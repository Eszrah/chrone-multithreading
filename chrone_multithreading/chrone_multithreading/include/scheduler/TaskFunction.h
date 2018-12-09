#pragma once

namespace chrone::multithreading::scheduler
{

struct Fiber;
struct Task;

struct TaskFunction
{
	static Fiber*	ExecuteTask(Task& task);
};

}

