#pragma once

#include "NativeType.h"
#include "TaskDecl.h"
#include "TaskSyncPrimitiveData.h"

namespace chrone::multithreading::scheduler
{

struct Task 
{
	Task() = default;
	Task(const Task&) = delete;
	Task(Task&& other) = default;
	~Task() = default;

	explicit Task(TaskDecl decl, 
		std::atomic<Fiber*>* dependencyFiber = nullptr,
		std::atomic<int>* dependencyCoouter = nullptr) :
		decl{std::move(decl)},
		dependencyFiber{ dependencyFiber },
		dependencyCoouter{ dependencyCoouter }
	{}

	Task&	operator=(const Task&) = delete;
	Task&	operator=(Task&& other) = default;

	inline bool Execute()
	{	
		decl.functor( decl.data );

		if (dependencyCoouter)
		{
			auto const	lastDependencyCount{ dependencyCoouter->fetch_sub(
				1u, std::memory_order_release) };

			return dependencyFiber && dependencyFiber->load(
				std::memory_order_relaxed) && (1u == lastDependencyCount);
		}

		return false;
	}

	inline bool CheckedExecute()
	{
		return decl.functor ? Execute() : false;
	}

	inline void Reset()
	{
		decl = TaskDecl{};
		dependencyFiber = nullptr;
		dependencyCoouter = nullptr;
	}

	inline Fiber* GetDependencyTask()
	{
		assert(dependencyFiber);
		return dependencyFiber->load(
			std::memory_order_relaxed);
	}

	TaskDecl	decl{};
	std::atomic<Fiber*>*	dependencyFiber{ nullptr };
	std::atomic<int>*	dependencyCoouter{ nullptr };
};

}