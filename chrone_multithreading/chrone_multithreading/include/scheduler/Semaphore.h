#pragma once

#include "NativeType.h"
#include <atomic>

namespace chrone::multithreading::scheduler
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

	std::atomic<Uint>	dependantCounter{};
	std::atomic<Fiber*>	dependantFiber{};
};

}