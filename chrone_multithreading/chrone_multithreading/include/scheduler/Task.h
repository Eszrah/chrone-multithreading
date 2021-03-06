#pragma once

#include "NativeType.h"
#include "TaskDecl.h"

namespace chrone::multithreading::fiberScheduler
{

struct Task
{
	Task() = default;
	Task(const Task&) = delete;
	Task(Task&&) = default;
	~Task() = default;

	Task(TaskDecl decl, Uint32 dependencyIndex) :
		decl{ decl },
		dependencyIndex{ dependencyIndex }
	{}

	Task&	operator=(const Task&) = delete;
	Task&	operator=(Task&&) = default;

	TaskDecl	decl;
	Uint32	dependencyIndex{ 0u };
};

}