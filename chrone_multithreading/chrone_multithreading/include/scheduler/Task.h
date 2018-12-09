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
	std::atomic<Uint>*	dependentCounter{ nullptr };
	Fiber*	dependentFiber{ nullptr };
	std::condition_variable*	fence{ nullptr };
};

struct Task
{
	TaskDecl	decl;
	TaskDependency	dependency;
};

}