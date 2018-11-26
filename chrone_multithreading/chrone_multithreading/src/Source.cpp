//#include "scheduler/fiber/FiberTaskScheduler.h"

#include <iostream>
#include <chrono>
#include <thread>
//
//using namespace chrone::multithreading::scheduler;
//
//constexpr unsigned int counter{500};
//
//struct Data
//{
//	int input0{};
//	int input1{};
//	int	result{};
//};
//
//struct Context
//{
//	FiberTaskSchedulerAgent*	agent{ nullptr };
//
//	unsigned int offset{ 500 };
//	void*	data{ nullptr };
//};

//void ProcessSumFunction(void* data)
//{
//	Data*	structData = static_cast<Data*>(data);
//	structData->result += structData->input0 + structData->input1;
//}
//
//void ProcessVoidFunction(void* data)
//{
//	Context*	context = static_cast<Context*>(data);
//	FiberTaskSchedulerAgent*	agent = context->agent;
//	Data*	structData = static_cast<Data*>(context->data);
//
//	TaskSyncPrimitive sync{};
//	agent->AllocateSyncPrimitive(sync);
//	TaskDecl	decl[counter]{};
//
//	for (auto index{ 0u }; index < counter; ++index)
//	{
//		decl[index] = { ProcessSumFunction,  structData + context->offset + index };
//	}
//
//	agent->PushTasks(counter, decl, &sync);
//	agent->Wait(sync);
//
//	agent->DeallocateSyncPrimitive(sync);
//
//
//}

//void ProcessVoidFunction(void* data)
//{
//	Context*	context = static_cast<Context*>(data);
//	FiberTaskSchedulerAgent*	agent = context->agent;
//	Data*	structData = static_cast<Data*>(context->data);
//	assert(structData);
//	structData[context->offset].result += structData[context->offset].input0 + structData[context->offset].input1;
//
//
//}

int main()
{
	//auto const logicalCoreCount{ std::thread::hardware_concurrency()};
	//FiberTaskScheduler	pool;

	//auto constexpr factor{ 1 };
	//auto constexpr declCount{ factor * counter };
	//if (!pool.Initialize(4, declCount))
	//{
	//	assert(false);
	//	return 0;
	//}
	//

	//TaskDecl	decl[declCount]{};
	//Data	data[declCount]{};
	//Context	ctx[declCount];
	// 
	//for (auto index{ 0u }; index < declCount; ++index)
	//{
	//	Data& currentData{ data[index] };
	//	currentData.input0 = index;
	//	currentData.input1 = index + index + 89 - 8 / 2;
	//	currentData.result = 0;
	//	ctx[index] = { pool.GetSchedulerAgent(), index, &data };
	//	decl[index] = { &ProcessVoidFunction, &ctx[index] };
	//}

	//for (auto index{ 0u }; index < factor; ++index)
	//{
	//	ctx[index] = { pool.GetSchedulerAgent(), index * counter, &data };
	//	decl[index] = {ProcessVoidFunction, &ctx[index] };
	//}


	//FiberTaskScheduler::SyncPrimitive syncPrim{};
	//pool.AllocateSyncPrimitive(syncPrim);
	//pool.PushTasks(declCount, decl, &syncPrim);


	//pool.Wait(syncPrim);

	//for (auto index{ 0u }; index < declCount; ++index)
	//{
	//	Data& currentData{ data[index] };
	//	assert((currentData.input0 + currentData.input1) == currentData.result);
	//}

	//pool.DeallocateSyncPrimitive(syncPrim);

	//while (true);

	//pool.Shutdown();


	return 0;
}
