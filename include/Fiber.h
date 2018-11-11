#pragma once

#include "NativeType.h"

namespace chrone::multithreading::scheduler::fiber
{

template<class KernelType>
struct Fiber
{
	Fiber() = default;
	Fiber(const Fiber&) = delete;
	Fiber(Fiber&&) = default;
	~Fiber() = default;

	Fiber(void* hFiber) :
		fiberHandle{hFiber}
	{}

	Fiber&	operator=(const Fiber&) = delete;
	Fiber&	operator=(Fiber&&) = default;

	inline Uint8	IsTemporary() { return propertiesBitmask & 0x1; }

	Uint8	propertiesBitmask{ 0u };
	void*	fiberHandle{ nullptr };
	KernelType	kernel{};
};

}