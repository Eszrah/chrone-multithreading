#pragma once

namespace chrone::multithreading::scheduler
{

struct Fiber;

struct ThreadFiberData
{
	Fiber*	waitingFiber{ nullptr };
	Fiber*	previousFiber{ nullptr };
	Fiber*	currentFiber{ nullptr };
};

}
