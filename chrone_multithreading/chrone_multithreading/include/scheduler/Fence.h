#pragma once

#include "NativeType.h"
#include <atomic>
#include <mutex>

namespace chrone::multithreading::scheduler
{

struct Fiber;

struct Fence
{
	std::mutex	mutex{};
	std::condition_variable	conditionVariable{};
	std::atomic<Uint>	dependantCounter{};
};

}