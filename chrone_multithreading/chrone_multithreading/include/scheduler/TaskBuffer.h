#pragma once

#include <vector>
#include "TaskDecl.h"

namespace chrone::multithreading::scheduler
{

struct TaskBuffer
{
	std::vector<TaskDecl>	tasks;
};

}