#pragma once

#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

struct TaskPool;
struct Task;
struct TaskDecl;

struct TaskPoolFunction 
{
	static bool	Initialize(TaskPool& pool, Uint32 maxTasksCountPowerOfTwo);
	static bool	Clear(TaskPool& pool);

	static bool	PushTasks(TaskPool& pool, Uint32 count, TaskDecl* tasksDecl, const Uint32 dependencyIndex);
	static bool	TryPopTask(TaskPool& pool, Task& task);
};

}