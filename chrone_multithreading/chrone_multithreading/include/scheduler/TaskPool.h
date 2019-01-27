#pragma once

#include <vector>

#include "NativeType.h"
#include "Spinlock.h"


namespace chrone::multithreading::fiberScheduler
{

struct Task;

struct TaskPool
{
	Spinlock	taskBuffersLock{};

	Uint32	headIndex{ 0u };
	Uint32	tailIndex{ 0u };
	Uint32	tasksMaxCount{ 0u };
	Task*	tasks{ nullptr };
};

}