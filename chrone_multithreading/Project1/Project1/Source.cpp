
	#include "Windows.h"
	#include <vector>
	#include <thread>
	#include <mutex>
	#include <cassert>
	#include <iostream>
	#include <atomic>

	struct Fiber
	{
		void* handle;
	};

	struct ThreadData
	{
		Fiber*	previousFiber{ nullptr };
		Fiber*	currentFiber{ nullptr };
		Fiber	fiber{ };
		unsigned int index{};
	};

	//Threads
	std::vector<std::pair<std::thread::id, unsigned int>> threadsinfo{};

	//threads data container
	ThreadData	threadsData[8];

	//Fibers
	std::mutex	fibersLock{};
	std::vector<Fiber> fibers{};
	std::vector<Fiber*> freeFibers{};

	thread_local unsigned int localThreadIndex{};
	thread_local Fiber* debug_localTheadLastFiber{};
	thread_local ThreadData* localThreadData{};

	using WindowsThread = HANDLE;
	std::vector<WindowsThread> threads{};

	//This is the first way to retrieve the current thread's ThreadData structure using thread_id
	ThreadData* GetThreadData()
	{
		std::thread::id threadId( std::this_thread::get_id());
		for (auto const& pair : threadsinfo)
		{
			if (pair.first == threadId)
			{
				return &threadsData[pair.second];
			}
		}
	
		//It is not possible to assert
		assert(false);
		return nullptr;
	}

	//This is the second way to retrieve the current thread's ThreadData structure using thread local storage
	//ThreadData* GetThreadData()
	//{
	//	return &threadsData[localThreadIndex];
	//}


	//This is the third way to retrieve the current thread's ThreadData structure using thread local storage
	//ThreadData* GetThreadData()
	//{
	//	return localThreadData;
	//}


	//Try to pop a free fiber from the container, thread safe due to mutex usage
	bool  TryPopFreeFiber(Fiber*& fiber)
	{
		std::lock_guard<std::mutex> guard(fibersLock);
		if (freeFibers.empty()) { return false; }
		fiber = freeFibers.back();
		assert(fiber);
		assert(fiber->handle);
		freeFibers.pop_back();
		return true;
	}


	//Try to push a free fiber to the container, thread safe due to mutex usage
	bool PushFreeFiber(Fiber* fiber)
	{
		std::lock_guard<std::mutex> guard(fibersLock);
		freeFibers.push_back(fiber);
		return true;
	}


	//the __declspec(noinline) is used to inspect code in release mode, comment it if you want
	__declspec(noinline) void  _SwitchToFiber(Fiber* newFiber)
	{
		//You want to switch to another fiber
		//You first have to save your current fiber instance to release it once you will be in the new fiber
		{
			ThreadData*	threadData{ GetThreadData() };
			assert(threadData->index == localThreadIndex);
			assert(threadData->currentFiber);
			threadData->previousFiber = threadData->currentFiber;
			threadData->currentFiber = newFiber;
			debug_localTheadLastFiber = threadData->previousFiber;
			assert(threadData->previousFiber);
			assert(newFiber);
			assert(newFiber->handle);
		}

		//You switch to the new fiber
		//this call will either make you enter in the FiberFunc function if the fiber has never been used
		//Or you will continue to execute this function if the new fiber has been already used (not that you will have a different stack so you can't use the old threadData value)
		::SwitchToFiber(newFiber->handle);

		{
			//You must get the current ThreadData* again, because you come from another fiber (the previous statement is a switch), this fiber could have been used by any other thread
			ThreadData*	threadData{ GetThreadData() };

			//THIS ASSERT WILL FIRES IF YOU USE THE FIRST GetThreadData METHOD, WHICH IS IMPOSSIBLE....
			assert(threadData->index == localThreadIndex);
		
			assert(threadData);
			assert(threadData->previousFiber);

			//We release the previous fiber
			PushFreeFiber(threadData->previousFiber);
			debug_localTheadLastFiber = nullptr;
			threadData->previousFiber = nullptr;
		}

	}


	void ExecuteThreadBody()
	{
		Fiber*	newFiber{};

		if (TryPopFreeFiber(newFiber))
		{
			_SwitchToFiber(newFiber);
		}
	}


	DWORD __stdcall ThreadFunc(void* data)
	{
		int const index{ *static_cast<int*>(data)};

		threadsinfo[index] = std::make_pair(std::this_thread::get_id(), index);

		//setting up the current thread data
		ThreadData*	threadData{ &threadsData[index] };
		threadData->index = index;

		void*	threadAsFiber{ ConvertThreadToFiber(nullptr) };
		assert(threadAsFiber);

		threadData->fiber = Fiber{ threadAsFiber };
		threadData->currentFiber = &threadData->fiber;

		localThreadData = threadData;
		localThreadIndex = index;

		while (true)
		{
			ExecuteThreadBody();
		}

		return DWORD{};
	}

	
	//The entry point of all fibers
	void __stdcall FiberFunc(void* data)
	{
		//You enter to the fiber for the first time

		ThreadData*	threadData{ GetThreadData() };

		//Making sure that the thread data structure is the good one
		assert(threadData->index == localThreadIndex);

		//Here you will assert
		assert(threadData->previousFiber);

		PushFreeFiber(threadData->previousFiber);
		threadData->previousFiber = nullptr;

		while (true)
		{
			ExecuteThreadBody();
		}
	}


	__declspec(noinline) void main()
	{
		constexpr unsigned int threadCount{ 2 };
		constexpr unsigned int fiberCount{ 20 };

		threadsinfo.resize(threadCount);

		fibers.resize(fiberCount);
		for (auto index = 0; index < fiberCount; ++index)
		{
			fibers[index] = { CreateFiber(0, FiberFunc, nullptr) };
		}

		freeFibers.resize(fiberCount);
		for (auto index = 0; index < fiberCount; ++index)
		{
			freeFibers[index] = std::addressof(fibers[index]);
		}

		threads.resize(threadCount);

		std::vector<int>	threadParamss(threadCount);



		for (auto index = 0; index < threadCount; ++index)
		{
			//threads[index] = new std::thread{ ThreadFunc, index };
			threadParamss[index] = index;
			threads[index] = CreateThread(NULL, 0, &ThreadFunc, &threadParamss[index], 0, NULL);
			assert(threads[index]);
		}

		while (true);

		//I know, it is not clean, it will leak
	}