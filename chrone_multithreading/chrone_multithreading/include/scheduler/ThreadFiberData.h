#pragma once

namespace chrone::multithreading::scheduler
{

struct Fiber;
struct Semaphore;

struct ThreadFiberData
{
	ThreadFiberData() = default;
	ThreadFiberData(const ThreadFiberData&) = default;
	ThreadFiberData(ThreadFiberData&&) = default;
	~ThreadFiberData() = default;

	ThreadFiberData&	operator=(const ThreadFiberData&) = default;
	ThreadFiberData&	operator=(ThreadFiberData&&) = default;

	Fiber*		syncSrcFiber{ nullptr };
	Semaphore*	syncSemaphore{ nullptr };
	Fiber*	previousFiber{ nullptr };
	Fiber*	currentFiber{ nullptr };
};

}
