#pragma once

#include "Task.h"

namespace chrone::multithreading::scheduler
{

struct TaskBatchNode
{
	TaskDependency	dependency{};
	Uint32	taskCount{ 0u };
	TaskDecl*	tasks{ nullptr };
	TaskBatchNode*	next{ nullptr };
};

}