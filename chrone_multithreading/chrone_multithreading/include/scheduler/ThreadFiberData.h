#pragma once

namespace chrone::multithreading::scheduler
{

struct Fiber;

struct ThreadFiberData
{
	Fiber*	previousFiber{ nullptr };
	Fiber*	currentFiber{ nullptr };
};

}
