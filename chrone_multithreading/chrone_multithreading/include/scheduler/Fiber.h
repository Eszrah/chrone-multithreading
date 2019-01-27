#pragma once

namespace std
{
	template<class T>
	struct atomic;
}

#include "NativeType.h"

namespace chrone::multithreading::fiberScheduler
{

struct FiberData;

struct Fiber
{
	Fiber() = default;
	Fiber(const Fiber&) = delete;
	Fiber(Fiber&&) = default;
	~Fiber() = default;

	Fiber(void*	fiberHandle, FiberData* fiberData):
		fiberData{ fiberData },
		fiberHandle{ fiberHandle }
	{}

	Fiber&	operator=(const Fiber&) = delete;
	Fiber&	operator=(Fiber&&) = default;

	FiberData*	fiberData{ nullptr };
	void*	fiberHandle{ nullptr };
};

}