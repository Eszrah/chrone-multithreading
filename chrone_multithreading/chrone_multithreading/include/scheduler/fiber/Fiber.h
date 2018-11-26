#pragma once

#include "NativeType.h"
#include "Task.h"

namespace chrone::multithreading::scheduler
{

struct Fiber
{
	Fiber() = default;
	Fiber(const Fiber&) = delete;
	Fiber(Fiber&&) = default;
	~Fiber() = default;

	explicit Fiber(void* hFiber) :
		fiberHandle{hFiber}
	{}

	Fiber&	operator=(const Fiber&) = delete;
	Fiber&	operator=(Fiber&&) = default;

	void*	fiberHandle{ nullptr };
	Task	task{};
};

/*
	inline bool Execute() { return task.Execute(); }
	inline bool CheckedExecute() { return task.CheckedExecute(); }
*/

}