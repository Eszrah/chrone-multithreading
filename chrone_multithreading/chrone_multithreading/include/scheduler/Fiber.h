#pragma once

#include "NativeType.h"
#include "TaskDecl.h"

namespace chrone::multithreading::scheduler
{

struct Fiber
{
	Fiber() = default;
	Fiber(const Fiber&) = delete;
	Fiber(Fiber&&) = default;
	~Fiber() = default;

	Fiber&	operator=(const Fiber&) = delete;
	Fiber&	operator=(Fiber&&) = default;

	void*	fiberHandle{ nullptr };
	TaskDecl	decl{};
	std::atomic<Fiber*>*	dependencyFiber{ nullptr };
	std::atomic<int>*	dependencyCoouter{ nullptr };
};

}