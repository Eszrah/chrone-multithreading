#pragma once

#include <vector>

#include "NativeType.h"
#include "Spinlock.h"
#include "TaskBuffer.h"

namespace chrone::multithreading::scheduler
{

struct TaskPool
{
	Spinlock	taskBuffersLock{};
	std::vector<TaskBuffer>	taskBuffers;
};

}