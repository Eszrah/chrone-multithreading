#pragma once

#include "NativeType.h"
#include "TaskDecl.h"
#include "TaskDependency.h"


namespace chrone::multithreading::scheduler
{

struct Task
{
	Task() = default;
	Task(const Task&) = delete;
	Task(Task&&) = default;
	~Task() = default;

	Task(TaskDecl decl, TaskDependency dependency) :
		decl{ decl },
		dependency{ dependency }
	{}

	Task&	operator=(const Task&) = delete;
	Task&	operator=(Task&&) = default;

	TaskDecl	decl;
	TaskDependency	dependency;
};

}