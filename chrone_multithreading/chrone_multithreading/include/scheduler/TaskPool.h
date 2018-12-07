#pragma once

#include <vector>

#include "NativeType.h"
#include "Spinlock.h"
#include "Task.h"

namespace chrone::multithreading::scheduler
{

struct TaskPool
{
	Spinlock	taskBuffersLock{};
	std::vector<Task>	taskBuffer;
};

}