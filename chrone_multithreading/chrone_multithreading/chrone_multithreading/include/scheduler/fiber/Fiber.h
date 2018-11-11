#pragma once

#include "NativeType.h"
#include "Task.h"

namespace chrone::multithreading::scheduler::fiber
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

	//inline bool	HasDependencies() const { 
	//	return task.dependencyCoouter->load(std::memory_order_acquire) != 0u; }

	inline bool Execute() { return task.Execute(); }
	inline bool CheckedExecute() { return task.CheckedExecute(); }

	void*	fiberHandle{ nullptr };
	Task	task{};
};

}