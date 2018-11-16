#pragma once

#include <atomic>
#include "FiberTaskSchedulerProxy.h"

namespace chrone::multithreading::scheduler
{

struct FiberFuncData
{
	FiberFuncData() = default;
	FiberFuncData(const FiberFuncData&) = delete;
	FiberFuncData(FiberFuncData&& other) = default;
	~FiberFuncData() = default;

	explicit FiberFuncData(
		FiberTaskSchedulerProxy schedulerProxy, 
		std::atomic_bool* keepRunning) :
		schedulerProxy{ schedulerProxy },
		keepRunning{ keepRunning }
	{}

	FiberFuncData&	operator=(const FiberFuncData&) = delete;
	FiberFuncData&	operator=(FiberFuncData&& other) = default;

	FiberTaskSchedulerProxy	schedulerProxy{};
	std::atomic_bool*	keepRunning{ nullptr };
};

}