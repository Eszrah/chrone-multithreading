#pragma once

#include <atomic>

namespace chrone::multithreading::scheduler
{

//struct FiberFuncData
//{
//	FiberFuncData() = default;
//	FiberFuncData(const FiberFuncData&) = delete;
//	FiberFuncData(FiberFuncData&& other) = default;
//	~FiberFuncData() = default;
//
//	explicit FiberFuncData(
//		FiberTaskSchedulerProxy schedulerProxy, 
//		std::atomic_bool* keepRunning) :
//		schedulerProxy{ schedulerProxy },
//		keepRunning{ keepRunning }
//	{}
//
//	FiberFuncData&	operator=(const FiberFuncData&) = delete;
//	FiberFuncData&	operator=(FiberFuncData&& other) = default;
//
//	FiberTaskSchedulerProxy	schedulerProxy{};
//	std::atomic_bool*	keepRunning{ nullptr };
//};

struct FiberData
{
	//another member to access tasks function
	Uint8				threadIndex{ 0xFFFFFFFF };
	std::atomic_bool*	keepRunning{ nullptr };
};

}