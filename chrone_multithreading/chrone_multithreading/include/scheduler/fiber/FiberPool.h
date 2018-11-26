#pragma once

#include <vector>
#include <algorithm>
#include <mutex>
#include <cassert>

#include "Fiber.h"
#include "NativeType.h"
#include "Spinlock.h"
//#include "WindowsFiberHelper.h"
//#include "TaskSyncPrimitiveData.h"
//#include "std_extension/SpinlockStdExt.h"

namespace chrone::multithreading::scheduler
{


struct FiberPool
{
	using SyncPrimitive = Spinlock;

	std::vector<Fiber>	fibers{};

	SyncPrimitive	freeFibersLock{};
	SyncPrimitive	readyFibersLock{};

	std::vector<Fiber*>	freeFibers{};
	std::vector<Fiber*>	readyFibers{};
};

/*
void
Reserve(
	Uint originalFiberCount,
	Uint totalFiberCount)
{
	fibers.reserve(originalFiberCount);
	freeFibers.reserve(totalFiberCount);
	readyFibers.reserve(totalFiberCount);
}

void
MakeAllFibersFree()
{
	auto const fibersCount{ fibers.size() };
	freeFibers.resize(fibersCount);

	Fiber*	fibersPtr{ fibers.data() };
	for (auto index{ 0u }; index < fibersCount; ++index, fibersPtr++)
	{
		freeFibers[index] = fibersPtr;
	}
}

void
Clear()
{
	for (auto index{ 0u }; index < fibers.size(); ++index)
	{
		WindowsFiberHelper::DeallocateFiber(fibers[index].fiberHandle);
	}

	fibers.clear();
	freeFibers.clear();
	readyFibers.clear();
}


bool
PushReadyTasks(
	Uint count,
	TaskDecl* tasks,
	std::atomic<Fiber*>* dependencyFiber = nullptr,
	AtomicCounter* dependencyCoouter = nullptr)
{
	Fiber*	freeFiber{ nullptr };

	for (auto index{ 0u }; index < count; ++index)
	{
		{
			std::lock_guard<MUTEX_TYPE>	guard{ freeFibersLock };
			freeFiber = freeFibers.back();
			freeFibers.pop_back();
			//assert(freeFiber);
		}

		//assert(freeFiber);
		freeFiber->task = Task{ std::move(tasks[index]), dependencyFiber,
			dependencyCoouter };

		{
			std::lock_guard<MUTEX_TYPE>	guard{ readyFibersLock };
			//assert(freeFiber->task.decl.functor);
			readyFibers.push_back(freeFiber);
		}
	}

	return true;
}

bool
PushFreeFiber(
	Fiber* fiber)
{
	std::lock_guard<MUTEX_TYPE>	guard{ freeFibersLock };
	//assert(fiber);
	freeFibers.emplace_back(fiber);
	return true;
}

bool
TryPopFreeFiber(
	Fiber*& fiber)
{
	std::lock_guard<MUTEX_TYPE>	guard{ freeFibersLock };
	if (freeFibers.empty()) { return false; }
	fiber = freeFibers.back();
	freeFibers.pop_back();
	return true;
}

bool
TryPopReadyFiber(
	Fiber*& fiber)
{
	std::lock_guard<MUTEX_TYPE>	guard{ readyFibersLock };
	if (readyFibers.empty()) { return false; }
	fiber = readyFibers.back();
	//assert(fiber);
	//assert(fiber->task.decl.functor);
	readyFibers.pop_back();
	return true;
}

template<class Pred>
Fiber*
GetFreeFiber(
	Pred&& pred)
{
	Fiber*	fiber{ nullptr };
	std::lock_guard<MUTEX_TYPE>	guard{ freeFibersLock };
	auto foundIt{ std::find_if(freeFibers.begin(), freeFibers.end(), pred) };

	if (foundIt == freeFibers.end()) { return fiber; }

	fiber = *foundIt;
	freeFibers.erase(foundIt);
	return fiber;
}*/


}