#pragma once

#include "NativeType.h"
#include <atomic>

namespace chrone::multithreading::scheduler
{

struct Semaphore
{
	std::atomic<Uint>	dependantCounter{};
};

}