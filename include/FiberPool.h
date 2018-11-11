#pragma once

#include "NativeType.h"
#include "Fiber.h"
#include "Spinlock.h"

namespace chrone::multithreading::scheduler::fiber
{

template<class TaskFunctor>
class FiberPool
{
public:


	bool	Initialize(Uint	fiberCount);
	bool	Shutdown();



	bool	PushAvailableFiber(Fiber* fiber);

	bool	PushTasks(Uint count, TaskFunctor* functors);
	bool	PopReadyFiber(Fiber*& fiber);


private:

	bool	_PopAvailableFibers(Uint count, Fiber** fibers);

	//All allocated fibers
	//fixed sized array ? 
	std::vector<Fiber>	fibers{};

	//container of available fibers
		//fixed sized array of fiber*
	Spinlock	availableFibersLock{};
	std::vector<Fiber*>	availableFibers{};
	
	//container of ready fibers
		//fixed sized array
	Spinlock	readyFibersLock{};
	std::vector<Fiber*>	readyFibers{};

	//container of functor which couldn't be be setup directly
	Spinlock	additionalFunctorsLock{};
	std::vector<TaskFunctor>	additionalFunctors{};
};

template<class TaskFunctor>
inline bool FiberPool<TaskFunctor>::PushAvailableFiber(Fiber * fiber)
{
	//LOCK AVAILABLE
	//push available fiber if this on has been specifically allocated on the heap

	return false;
}

template<class TaskFunctor>
inline bool FiberPool<TaskFunctor>::PushTasks(Uint count, TaskFunctor * functors)
{
	//LOCK AVAILABLE
	//take count fibers with _PopAvailableFibers

	//setup those fibers

	//LOCK READY
	//push count ready fibers


	return false;
}

template<class TaskFunctor>
inline bool FiberPool<TaskFunctor>::PopReadyFiber(Fiber *& fiber)
{
	//LOCK
	//increment the index of front

	return false;
}

template<class TaskFunctor>
inline bool FiberPool<TaskFunctor>::_PopAvailableFibers(Uint count, Fiber ** fibers)
{
	//LOCK AVAILABLE
	//if not enough fibers, allocate new one in a specific container, on the HEAP

	return false;
}

}