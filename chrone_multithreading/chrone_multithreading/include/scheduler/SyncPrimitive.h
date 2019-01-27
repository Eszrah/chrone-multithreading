#pragma once

#include <atomic>
#include <mutex>

#include "NativeType.h"
#include "HSemaphore.h"


namespace chrone::multithreading::fiberScheduler
{

struct Fiber;

struct Semaphore
{
	Semaphore() = default;
	Semaphore(const Semaphore& other) = delete;
	Semaphore(Semaphore&&) = default;
	~Semaphore() = default;

	Semaphore&	operator=(const Semaphore&) = delete;
	Semaphore&	operator=(Semaphore&&) = default;

	std::atomic<Uint>	dependentCounter{};
	std::atomic<Fiber*>	dependentFiber{};
};


struct Fence
{
	Fence() = default;
	Fence(const Fence&) = delete;
	Fence(Fence&&) = default;
	~Fence() = default;

	Fence&	operator=(const Fence&) = delete;
	Fence&	operator=(Fence&&) = default;

	std::mutex	mutex{};
	Uint8	fenceSignaled{ false };
	std::condition_variable	conditionVariable{};
	HSemaphore	hSemaphore{};
};

}