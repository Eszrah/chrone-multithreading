#pragma once

#include <vector>
#include <algorithm>
#include <mutex>
#include <cassert>

#include "Fiber.h"
#include "NativeType.h"
#include "Spinlock.h"
#include "WindowsFiberHelper.h"
#include "TaskSyncPrimitiveData.h"
#include "std_extension/SpinlockStdExt.h"

//#define USE_CUSTOM_MUTEX

namespace chrone::multithreading::scheduler::fiber
{

#ifdef USE_CUSTOM_MUTEX
#define MUTEX_TYPE Spinlock
#else
#define MUTEX_TYPE std::mutex
#endif // USE_CUSTOM_MUTEX


struct FiberPool
{
	using AtomicCounter = std::atomic<int>;

	__declspec(noinline) void	
	Reserve(
	Uint originalFiberCount, 
	Uint totalFiberCount)
	{
		fibers.reserve(originalFiberCount);
		freeFibers.reserve(totalFiberCount);
		readyFibers.reserve(totalFiberCount);
		waitingFibers.reserve(totalFiberCount);
	}

	__declspec(noinline) void 
	MakeAllFibersFree()
	{
		auto const fibersCount{ fibers.size() };
		freeFibers.resize(fibersCount);

		Fiber*	fibersPtr{ fibers.data() };
		for (auto index{ 0u }; index < fibersCount; ++index)
		{
			freeFibers[index] = fibersPtr++;
		}
	}

	__declspec(noinline) void	
	Clear()
	{
		for (auto index{ 0u }; index < fibers.size(); ++index)
		{
			WindowsFiberHelper::DeallocateFiber(fibers[index].fiberHandle);
		}
		
		fibers.clear();
		freeFibers.clear();
		readyFibers.clear();
		waitingFibers.clear();
	}


	bool	
	PushReadyTasks(
		Uint count, 
		TaskDecl* tasks,
		std::atomic<Fiber*>* dependencyFiber = nullptr,
		AtomicCounter* dependencyCoouter = nullptr)
	{
		for (auto index{ 0u }; index < count; ++index)
		{
			Fiber*	freeFiber{ nullptr };
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
				std::lock_guard<MUTEX_TYPE>	guard2{ readyFibersLock };
				//assert(freeFiber->task.decl.functor);
				try
				{
					readyFibers.push_back(freeFiber);
				}
				catch (...)
				{
					//assert(readyFibers.back()->task.decl.functor);
				}
				//assert(readyFibers.back()->task.decl.functor);
			}
		}

		return true;
	}

	__declspec(noinline) bool 
	PushFreeFiber(
		Fiber* fiber)
	{
		std::lock_guard<MUTEX_TYPE>	guard{ freeFibersLock };
		//assert(fiber);
		freeFibers.emplace_back(fiber);
		return true;
	}

	__declspec(noinline) bool
	TryPopFreeFiber(
			Fiber*& fiber)
	{
		std::lock_guard<MUTEX_TYPE>	guard{ freeFibersLock };
		if (freeFibers.empty()) { return false; }
		fiber = freeFibers.back();
		freeFibers.pop_back();
		return true;
	}

	__declspec(noinline) bool
	TryPopReadyFiber(
		Fiber*& fiber)
	{
		std::lock_guard<MUTEX_TYPE>	guard{ readyFibersLock };
		if (readyFibers.empty()) { return false; }
		fiber = readyFibers.back();
		/*assert(fiber);
		assert(fiber->task.decl.functor);*/
		readyFibers.pop_back();	
		return true;
	}


	__declspec(noinline) bool 
	PushWaitingFiber(
		Fiber* fiber)
	{
		std::lock_guard<MUTEX_TYPE>	guard{ waitingFiberssLock };
		//assert(fiber);
		waitingFibers.emplace_back(fiber);
		return true;
	}
	
	__declspec(noinline) bool 
	TryPopWaitingFiber(
			Fiber*& fiber)
	{
		std::lock_guard<MUTEX_TYPE>	guard{ waitingFiberssLock };
		if (waitingFibers.empty()) { return false; }
		fiber = waitingFibers.back();
		//assert(fiber);
		waitingFibers.pop_back();
		return true;
	}

	template<class Pred>
	__declspec(noinline) Fiber* 
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
	}

	std::vector<Fiber>	fibers{};

	MUTEX_TYPE	freeFibersLock{};
	MUTEX_TYPE	readyFibersLock{};
	MUTEX_TYPE	waitingFiberssLock{};

	std::vector<Fiber*>	freeFibers{};
	std::vector<Fiber*>	readyFibers{};
	std::vector<Fiber*>	waitingFibers{};
};

}