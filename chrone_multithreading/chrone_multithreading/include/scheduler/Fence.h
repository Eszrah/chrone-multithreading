#pragma once

#include "NativeType.h"
#include <atomic>
#include <mutex>

namespace chrone::multithreading::scheduler
{

struct Fiber;

struct Fence
{
	Fence() = default;
	Fence(const Fence&) = delete;
	Fence(Fence&&) = default;
	~Fence() = default;

	Fence&	operator=(const Fence&) = delete;
	Fence&	operator=(Fence&&) = default;


	std::mutex	mutex{};
	std::condition_variable	conditionVariable{};
	std::atomic<Uint>	dependantCounter{};
};

}