#pragma once

#include <vector>

#include "NativeType.h"
#include "Spinlock.h"
#include "Task.h"

namespace chrone::multithreading::scheduler
{

struct TaskNode 
{
	Task	task{};
	TaskNode*	next{ nullptr };
};

struct TaskNodeList 
{
	TaskNode*	begin{ nullptr };
	TaskNode*	end{ nullptr };
};

struct TaskPool
{
	Spinlock	taskBuffersLock{};
	TaskNodeList	tasks{};
	/*std::vector<Task>	taskBuffer;*/
};

}