#pragma once

namespace chrone::multithreading::scheduler
{

struct TaskPool;
struct TaskDecl;

struct TaskPoolFunction 
{
	static bool	Initialize(TaskPool& pool);
	static bool	Shutdown(TaskPool& pool);

	static bool	TryGetTask(TaskPool& pool, TaskDecl& task);
};

}