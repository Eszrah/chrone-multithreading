#include "scheduler/WorkItemFunction.h"

#include <cassert>

#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/FiberPoolFunction.h"
#include "scheduler/TaskPoolFunction.h"
#include "scheduler/FiberFunction.h"
#include "scheduler/Fiber.h"
#include "scheduler/Task.h"
#include "scheduler/SyncPrimitive.h"

namespace chrone::multithreading::scheduler
{

void 
WorkItemFunction::MainLoop(
	FiberTaskSchedulerData& scheduler)
{
	std::atomic_bool&	threadsKeepRunning{
		scheduler.threadsData.threadsKeepRunning };

	std::vector<ThreadFiberData>&	threadFibersData{
		scheduler.threadFibersData };

	FiberPool&	fiberPool{ scheduler.fiberPool };
	TaskPool&	taskPool{ scheduler.taskPool };

	Semaphore* const	semaphores{ scheduler.semaphores };

	Uint8	threadIndex{ FiberFunction::GetFiberData()->threadIndex };
	ThreadFiberData&	threadFiberData{ threadFibersData[threadIndex] };

	Task	currentTask{};
	Fiber*	dependentFiber{ nullptr };

	while (threadsKeepRunning.load(std::memory_order_acquire))
	{
		if (dependentFiber)
		{
			FiberFunction::SwitchToFiber(fiberPool, threadFiberData, dependentFiber);

			//you may have a previous fiber set to null if you switch with the function WaitToFiber
			//=> deport the code below in the switch to fiber function
			assert(false);

			threadIndex = FiberFunction::GetFiberData()->threadIndex;
			threadFiberData = threadFibersData[threadIndex];
			assert(threadFiberData.previousFiber);
			FiberPoolFunction::PushFreeFiber(fiberPool, threadFiberData.previousFiber);
			threadFiberData.previousFiber = nullptr;
		}

		if (!TaskPoolFunction::TryPopTask(taskPool, currentTask))
		{
			//Wait ???
			assert(false);
			continue;
		}

		const TaskDecl&	decl{ currentTask.decl };
		Semaphore&		dependency{ semaphores[currentTask.dependencyIndex] };

		decl.functor(decl.data);
		assert(false); //=> call the functor in a wrapper to provide argument without memory allocation

		Fiber*	dependentFiber{ dependency.dependentFiber.load(std::memory_order_acquire) };
		
		//*/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\*//
		//An IMPORTANT note is that two threads can retrieve the same value
		//If the first thread decrement, then the thread which pushed this job increment, and another thread decrement
		//But there is no harm in our case, only one thread can see the 0 value due to memory ordering
		//*/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\*//
		//We decrement the shared counter
		auto const	lastDependencyCount{ dependency.dependentCounter.fetch_sub(
			1u, std::memory_order_release) }; //

		if (dependentFiber && 
			lastDependencyCount == 0)
		{
			//We have synchronize-with the release store performed by the thread who performed the wait operation
			//We are also sure that


			//We retrieve the dependent fiber

			if (dependentFiber)
			{
				//*/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\*//
				//Remember that you can have two threads which reads 0 and reach this point
				//*/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\*//

				
			}
		}


		//Decreasing the global counter
		/*
		A read-modify-write operation with this memory order is both an acquire operation and a release operation. 
		No memory reads or writes in the current thread can be reordered before or after this store. 
		All writes in other threads that release the same atomic variable are visible before the modification 
		and the modification is visible in other threads that acquire the same atomic variable.
		*/
		auto const	lastDependencyCount{ dependency.dependentCounter.fetch_sub(
			1u, std::memory_order_acq_rel) }; //

		//to be considered seriously
		assert(false); //=> how coult I be sure that the fetch part can synchronize with the add ???? should i use acq_release ?
		//we have to make sure the fetch sub can synchronize-with the store fetch add operation performed by the thread who pushed use the semaphore
		
		//we are now sure that we CAN synchronize with the fetch-add operation performed by the "waiting thread" (it there is one)
		if (lastDependencyCount == 0u)
		{
			//all the load are now at least as recent as the lastDependencyCount value
			//Fiber*	dependentFiber{ dependency.dependentFiber.load(std::memory_order_relaxed) };

			assert(false);//think twice about code validity
			if (dependentFiber)
			{
				//We switch to it
			}
		}

		//dependentFiber = lastDependencyCount == 1u ? dependency.dependentFiber.load(std::memory_order_relaxed) : nullptr;
	}
}

}