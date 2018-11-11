#pragma once

#include <atomic>
#include "FiberTaskSchedulerProxy.h"

namespace chrone::multithreading::scheduler::fiber
{

struct FiberData
{
	FiberData() = default;
	FiberData(const FiberData&) = delete;
	FiberData(FiberData&& other) = default;
	~FiberData() = default;

	explicit FiberData(
		FiberTaskSchedulerProxy schedulerProxy, 
		std::atomic_bool* keepRunning) :
		schedulerProxy{ schedulerProxy },
		keepRunning{ keepRunning }
	{}

	FiberData&	operator=(const FiberData&) = delete;
	FiberData&	operator=(FiberData&& other) = default;

	FiberTaskSchedulerProxy	schedulerProxy{};
	std::atomic_bool*	keepRunning{ nullptr };
};

}