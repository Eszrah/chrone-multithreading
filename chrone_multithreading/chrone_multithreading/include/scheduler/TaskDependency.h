#pragma once

#include "NativeType.h"
#include "TaskDecl.h"

namespace std
{
template<class T>
struct atomic;

class condition_variable;
}

namespace chrone::multithreading::scheduler
{

struct Fiber;

struct TaskDependency
{
	TaskDependency() = default;
	TaskDependency(const TaskDependency&) = default;
	TaskDependency(TaskDependency&&) = default;
	~TaskDependency() = default;

	TaskDependency&	operator=(const TaskDependency&) = default;
	TaskDependency&	operator=(TaskDependency&&) = default;

	std::atomic<Uint>*	dependentCounter{ nullptr };
	std::atomic<Fiber*>*	dependentFiber{ nullptr };
	std::condition_variable*	fence{ nullptr };
};

}