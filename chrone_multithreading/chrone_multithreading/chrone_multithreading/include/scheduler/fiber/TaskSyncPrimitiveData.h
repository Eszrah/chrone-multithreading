#pragma once

#include "NativeType.h"
#include <atomic>


namespace chrone::multithreading::scheduler::fiber
{

struct Fiber;


struct TaskSyncPrimitiveData 
{
	std::atomic<Fiber*>	dependencyFiber{ nullptr };
	std::atomic<int>	dependencyCoouter{ 0u };
};

}