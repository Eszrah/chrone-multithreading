#pragma once

#include <vector>

#include "NativeType.h"
#include "Spinlock.h"
#include "TaskNodeList.h"

namespace chrone::multithreading::scheduler
{

struct TaskPool
{
	Spinlock	taskBuffersLock{};

	Uint32	headIndex{ 0u };
	Uint32	tailIndex{ 0u };
	Uint32	tasksMaxCount{ 0u };
	Task*	tasks{ nullptr };
};

}