#pragma once

#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

struct TaskPool;
struct TaskDecl;
struct Task;
struct TaskDependency;

struct TaskPoolFunction 
{
	static bool	Initialize(TaskPool& pool);
	static bool	Shutdown(TaskPool& pool);

	static bool	PushTasks(TaskPool& pool, Uint32 count, const TaskDecl* tasksDecl, TaskDependency dependency);
	static bool	TryGetTask(TaskPool& pool, Task& task);
};

}