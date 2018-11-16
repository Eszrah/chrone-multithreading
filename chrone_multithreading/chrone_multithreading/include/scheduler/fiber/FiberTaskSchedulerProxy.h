#pragma once

namespace chrone::multithreading::scheduler
{

class FiberTaskScheduler;
struct Fiber;

class FiberTaskSchedulerProxy
{
public:
	FiberTaskSchedulerProxy() = default;
	explicit FiberTaskSchedulerProxy(FiberTaskScheduler* scheduler) noexcept:
		_scheduler{ scheduler }
	{}
	FiberTaskSchedulerProxy(const FiberTaskSchedulerProxy&) = default;
	FiberTaskSchedulerProxy(FiberTaskSchedulerProxy&& other) = default;
	~FiberTaskSchedulerProxy() = default;


	FiberTaskSchedulerProxy&	operator=(const FiberTaskSchedulerProxy&) = default;
	FiberTaskSchedulerProxy&	operator=(FiberTaskSchedulerProxy&& other) = default;

	void	DebugEntryCheck();
	void	InitFiberFirstEntry();
	void	ExecuteTask();
	void	SwitchBackToOriginalThread();

private:
	
	FiberTaskScheduler*	_scheduler{ nullptr };
};

}