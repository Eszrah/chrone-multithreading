#pragma once

#include <vector>
#include "TaskNodeList.h"

namespace chrone::multithreading::scheduler
{

struct TaskBuffer
{
	std::vector<TaskDecl>	tasks;
};

}