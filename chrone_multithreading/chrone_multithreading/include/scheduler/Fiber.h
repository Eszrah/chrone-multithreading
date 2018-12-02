#pragma once

namespace std
{
	template<class T>
	struct atomic;
}

#include "NativeType.h"
#include "TaskDecl.h"
#include "DummyTaskFunctor.h"

namespace chrone::multithreading::scheduler
{

struct FiberData;
struct Fiber;

struct Task
{
	Task() = default;
	Task(const Task&) = delete;
	Task(Task&&) = default;
	~Task() = default;

	Task&	operator=(const Task&) = delete;
	Task&	operator=(Task&&) = default;

	TaskDecl	decl{ DummyTaskFunctor, nullptr };
	std::atomic<Fiber*>*	dependencyFiber{ nullptr };
	std::atomic<int>*	dependencyCoouter{ nullptr };
};

struct Fiber
{
	Fiber() = default;
	Fiber(const Fiber&) = delete;
	Fiber(Fiber&&) = default;
	~Fiber() = default;

	Fiber(void*	fiberHandle, FiberData* fiberData):
		fiberData{ fiberData },
		fiberHandle{ fiberHandle },
		task{}
	{}

	Fiber&	operator=(const Fiber&) = delete;
	Fiber&	operator=(Fiber&&) = default;

	FiberData*	fiberData{ nullptr };
	void*	fiberHandle{ nullptr };
	Task	task{};
};

}