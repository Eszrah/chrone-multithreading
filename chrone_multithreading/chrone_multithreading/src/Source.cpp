#include "scheduler/FiberTaskSchedulerData.h"
#include "scheduler/TaskDecl.h"
#include "scheduler/FiberTaskSchedulerFunction.h"
#include "scheduler/FiberTaskSchedulerExternalFunction.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>

using namespace chrone::multithreading::fiberScheduler;

struct SumData
{
	Uint index;
	Int*	src0;
	Int*	src1;
	Int*	dst;
};

void SumJob(void* data)
{
	SumData*	sumData = static_cast<SumData*>(data);
	Uint index = sumData->index;


	sumData->dst[index] = sumData->src0[index] + sumData->src1[index];
}


int main()
{
	TaskSchedulerData	scheduler;
	const Uint16 threadCount = 1;
	const Uint16 fiberCount = 100;
	const Uint32 maxTaskCountPowerOfTwo = 256;
	const Uint16 fenceMaxCount = 2;
	const Uint16 semaphroeMaxCount = 2;

	const Uint count = 400;
	Int*	src0 = new Int[count];
	Int*	src1 = new Int[count];
	Int*	dst  = new Int[count];


	SumData	sumData[count];
	TaskDecl	decls[count];

	for (Uint index = 0; index < count; ++index)
	{
		src0[index] = index;
		src1[index] = index;
		sumData[index].index = index;
		sumData[index].src0 = src0;
		sumData[index].src1 = src1;
		sumData[index].dst = dst;
		decls[index] = TaskDecl{ SumJob , &sumData[index] };
	}


	TaskSchedulerFunction::Initialize(scheduler, threadCount, fiberCount, maxTaskCountPowerOfTwo, fenceMaxCount, semaphroeMaxCount);

	HFence hFence = TaskSchedulerFunction::AllocateFence(scheduler);

	TaskSchedulerExternalFunction::SubmitTasks(scheduler, count, decls, hFence);
	TaskSchedulerExternalFunction::WaitFence(scheduler, hFence);

	for (Uint index = 0; index < count; ++index)
	{
		assert(dst[index] == src0[index] + src1[index]);
	}

	TaskSchedulerFunction::DeallocateFence(scheduler, hFence);
	TaskSchedulerFunction::Shutdown(scheduler);

	return 0;
}
