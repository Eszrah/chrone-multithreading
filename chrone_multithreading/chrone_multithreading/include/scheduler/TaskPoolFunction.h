#pragma once

#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

struct TaskPool;
struct TaskNodeList;
struct Task;
struct TaskDependency;

struct TaskPoolFunction 
{
	static bool	Initialize(TaskPool& pool);
	static bool	Shutdown(TaskPool& pool);

	static bool	PushTasks(TaskPool& pool, Uint32 count, TaskNodeList pushedTaskList, TaskDependency dependency);
	static bool	TryPopTask(TaskPool& pool, Task& task);
};

}