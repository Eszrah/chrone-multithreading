#pragma once

#include <vector>
#include <thread>
#include <algorithm>
#include <future>

#include "NativeType.h"
#include "Fiber.h"
#include "Spinlock.h"
#include "WindowsFiberHelper.h"

thread_local Uint8	localThreadIndex{ 0u };

namespace chrone::multithreading::scheduler::fiber
{

template<class KernelType>
class FiberTaskScheduler
{
public:

	FiberTaskScheduler() = default;
	FiberTaskScheduler(const FiberTaskScheduler&) = delete;
	FiberTaskScheduler(FiberTaskScheduler&&) = delete;
	~FiberTaskScheduler() = default;

	FiberTaskScheduler&	operator=(const FiberTaskScheduler&) = delete;
	FiberTaskScheduler&	operator=(FiberTaskScheduler&&) = delete;

	template<class Func, class... Args>
	bool	Launch(Uint8 threadCount, Uint32 fiberCount, Func&& func, Args&&... args);
	
	bool	WaitCompletion();

private:

	using FiberType = Fiber<KernelType>;
	using FiberPtr = FiberType*;
	using FiberVector = std::vector<FiberType>;
	using FiberPtrVector = std::vector<FiberType>;
	using KernelVector = std::vector<KernelType>;
	using CoreThread = std::thread;
	using UniqueCoreThread = std::unique_ptr<CoreThread>;
	using SyncPrimitiveType = Spinlock;

	enum struct ETHREAD_SIGNAL : Uint8
	{
		WAIT, ABORT, GO
	};

	enum struct ETHREAD_OP_RESULT : Uint8
	{
		OK, ERROR
	};

	struct ThreadData final
	{
		ThreadData() = default;
		
		inline ThreadData(const ThreadData& other)
		{
			result.store(other.result.load());
			originalFiber = other.originalFiber;
			currentFiber = other.currentFiber;
		}

		ThreadData(ThreadData&& other)
		{
			result.store(other.result.load());
			originalFiber = other, originalFiber;
			currentFiber = other.currentFiber;
		}

		~ThreadData() = default;

		ThreadData&	operator=(const ThreadData&) = delete;
		inline ThreadData&	operator=(ThreadData&& other)
		{
			result.store(other.result.load());
			originalFiber = other,originalFiber;
			currentFiber = other.currentFiber;
			return *this;
		}


		std::atomic<ETHREAD_OP_RESULT>	result{ ETHREAD_OP_RESULT::OK };
		FiberType*	originalFiber{ nullptr };
		FiberType*	currentFiber{ nullptr };
	};

	struct UserMainThreadExecData 
	{
		UserMainThreadExecData() = default;
		UserMainThreadExecData(const UserMainThreadExecData&) = delete;
		
		UserMainThreadExecData(UserMainThreadExecData&& other)
		{
			mainThreadSignal = other.mainThreadSignal.load();
			workerThreadKeepRunning = other.workerThreadKeepRunning.load();
			workerThreadCounter = other.workerThreadCounter.load();
			threadCount = other.threadCount;
			fiberCount = other.fiberCount;
		}

		~UserMainThreadExecData() = default;

		UserMainThreadExecData(Uint8 threadCount, Uint32 fiberCount):
			threadCount{ threadCount },
			fiberCount{ fiberCount }
		{}

		UserMainThreadExecData&	operator=(const UserMainThreadExecData&) = delete;
		inline UserMainThreadExecData&	operator=(UserMainThreadExecData&& other)
		{
			mainThreadSignal = other.mainThreadSignal.load();
			workerThreadKeepRunning = other.workerThreadKeepRunning.load();
			workerThreadCounter = other.workerThreadCounter.load();
			threadCount = other.threadCount;
			fiberCount = other.fiberCount;
			return *this;
		}

		std::atomic<ETHREAD_SIGNAL>	mainThreadSignal{ ETHREAD_SIGNAL::WAIT };
		std::atomic_bool	workerThreadKeepRunning{ true };
		std::atomic<Uint8>	workerThreadCounter{ 0u };
		Uint8 threadCount{ 0u };
		Uint32 fiberCount{ 0u };
	};

	struct WorkerThreadExecData 
	{
		WorkerThreadExecData() = default;
		WorkerThreadExecData(const WorkerThreadExecData&) = delete;
		WorkerThreadExecData(WorkerThreadExecData&& other)
		{
			threadIndex = other.threadIndex;
			startValidationCount = other.startValidationCount;
			mainThreadSignal = other.mainThreadSignal;
			keepRunning = other.keepRunning;
		}
		~WorkerThreadExecData() = default;

		WorkerThreadExecData(
			Uint8 threadIndex,
			std::atomic<Uint8>* startValidationCount,
			std::atomic<ETHREAD_SIGNAL> const* mainThreadSignal,
			std::atomic_bool* keepRunning) :
			threadIndex{ threadIndex },
			startValidationCount{ startValidationCount },
			mainThreadSignal{ mainThreadSignal },
			keepRunning{ keepRunning }
		{}

		WorkerThreadExecData&	operator=(const WorkerThreadExecData&) = delete;
		WorkerThreadExecData&	operator=(WorkerThreadExecData&& other)
		{
			threadIndex = other.threadIndex;
			startValidationCount = other.startValidationCount;
			mainThreadSignal = other.mainThreadSignal;
			keepRunning = other.keepRunning;
			return *this;
		}

		Uint8	threadIndex{ 0u };
		std::atomic<Uint8>*	startValidationCount{ nullptr };
		std::atomic<ETHREAD_SIGNAL> const*	mainThreadSignal{ nullptr };
		std::atomic_bool*	keepRunning{ nullptr };
	};

	//struct FiberExecData 
	//{
	//	std::atomic_bool	keepRunning{ true; };
	//	FiberTaskScheduler*	scheduler{ nullptr };
	//};

	template<class Func, class... Args>
	void	_UserMainThreadFunc(Func&& func, Args&&... args);
	bool	_InitializeUserMainThread();
	bool	_LaunchWorkerThreads();
	void	_JoinUserMainThread();

	void	_WorkerThreadFunc(WorkerThreadExecData threadData);
	bool	_InitializeWorkerThread(const WorkerThreadExecData& workerThreadData);
	void	_JoinWorkerThreads();

	inline bool	_IsRunning() const { return _isRunning; }
	inline void	_SetLaunchState(bool launch) { _isRunning = launch; };
	bool	_Initialize(Uint8 threadCount, Uint32 fiberCount);
	
	static void _FiberFunc(void* data) {}

	bool	_isRunning{ false };
	UserMainThreadExecData	_MTExecData{};
	std::vector<ThreadData>	_threadsData{};
	std::vector<UniqueCoreThread>	_coreThreads{};

	/*std::vector<ThreadData>	_workerThreadData{};*/

	//fixed size container of allocated Task
	FiberVector	_fibers{};
};

struct TestStruct
{
	static void TestFunc() {}
};


template<class KernelType>
template<class Func, class... Args>
inline bool FiberTaskScheduler<KernelType>::Launch(Uint8 threadCount, Uint32 fiberCount, Func&& func, Args&&... args)
{
	if (!_Initialize(threadCount, fiberCount)) { return false; }

	//Launching the "User main thread"
	_coreThreads[0] = UniqueCoreThread{ new CoreThread{ [&] { _UserMainThreadFunc(std::forward<Func>(func), std::forward<Args>(args)...); }} };

	//Checking everything have been well launched
	while (true);

	return true;
}

template<class KernelType>
inline bool FiberTaskScheduler<KernelType>::WaitCompletion()
{

	return false;
}

template<class KernelType>
template<class Func, class... Args>
inline void FiberTaskScheduler<KernelType>::_UserMainThreadFunc(Func&& func, Args&&... args)
{
	bool const	initResult{ _InitializeUserMainThread() };

	if (!initResult)
	{
		//SIGNAL THE MAIN THREAD YOU FAILED
		return;
	}

	//Call the user function
	func(std::forward<Args>(args)...);

	//Joining the worker threads
	_JoinWorkerThreads();
}

template<class KernelType>
inline bool FiberTaskScheduler<KernelType>::_InitializeUserMainThread()
{
	void*	threadFiber = WindowsFiberHelper::ConvertThreadToFiber(nullptr);

	localThreadIndex = 0u;
	ThreadData&	threadData{ _threadsData[localThreadIndex] };

	if (!threadFiber)
	{
		threadData.result = ETHREAD_OP_RESULT::ERROR;
		return false;
	}

	_fibers[localThreadIndex] = FiberType{ threadFiber };

	if (!_LaunchWorkerThreads())
	{
		return false;
	}

	//Creating all the fibers
	Uint32 const	nativefiberOffset{ _MTExecData.threadCount };

	for (auto index{ nativefiberOffset };
		index < nativefiberOffset + _MTExecData.fiberCount; ++index)
	{
		_fibers[index] = FiberType{
			WindowsFiberHelper::AllocateHFiber(0u, &FiberTaskScheduler<KernelType>::_FiberFunc, nullptr) };
	}

	_MTExecData.workerThreadCounter = 0;
	_MTExecData.mainThreadSignal = ETHREAD_SIGNAL::GO;

	return true;
}

template<class KernelType>
inline bool	FiberTaskScheduler<KernelType>::_LaunchWorkerThreads()
{
	const auto	threadCount{ _MTExecData.threadCount };
	const auto	workerThreadCount{ _MTExecData.threadCount - 1u };
	if (!workerThreadCount) { return true; }

	//Launching the worker threads
	for (Uint8 index{ 1u }; index < (threadCount); ++index)
	{
		_coreThreads[index] = UniqueCoreThread
		{
			new CoreThread
			{
				[this, index] { _WorkerThreadFunc(WorkerThreadExecData{
				index, std::addressof(_MTExecData.workerThreadCounter),
				std::addressof(_MTExecData.mainThreadSignal),
				std::addressof(_MTExecData.workerThreadKeepRunning)}); }
			} 
		};
	}

	//Waiting for all threads to finish their initialization
	while (_MTExecData.workerThreadCounter != workerThreadCount);

	bool const	checkAnyThreadFailedInit{
		std::any_of(_threadsData.begin() + 1, _threadsData.end(),
		[](const ThreadData& threadData)
	{ return threadData.result == ETHREAD_OP_RESULT::ERROR; }) };


	if (checkAnyThreadFailedInit)
	{
		_MTExecData.mainThreadSignal = ETHREAD_SIGNAL::ABORT;
		_JoinWorkerThreads();
		return false;
	}

	return true;
}


template<class KernelType>
inline void FiberTaskScheduler<KernelType>::_JoinUserMainThread()
{
	if (_coreThreads.size() > 0)
	{
		_coreThreads[0]->join();
	}
}

template<class KernelType>
inline void FiberTaskScheduler<KernelType>::_WorkerThreadFunc(WorkerThreadExecData workerThreadData)
{
	bool const	initResult{ _InitializeWorkerThread(workerThreadData) };

	if (!initResult)
	{
		return;
	}

	auto const&	mainThreadSignal{ *workerThreadData.mainThreadSignal };

	//Waiting until the start signal has been sent without abort signal 
	while (auto const currentSignal =
		mainThreadSignal.load(std::memory_order_acquire) !=
		ETHREAD_SIGNAL::GO)
	{
		if (mainThreadSignal == ETHREAD_SIGNAL::ABORT) { return; }
	}

	//Launching the worker thread loop
}


template<class KernelType>
inline bool FiberTaskScheduler<KernelType>::_InitializeWorkerThread(const WorkerThreadExecData& workerThreadData)
{
	localThreadIndex = workerThreadData.threadIndex;
	ThreadData&	threadData{ _threadsData[localThreadIndex] };
	void*	threadFiber{ WindowsFiberHelper::ConvertThreadToFiber(nullptr) };

	auto const&	mainThreadSignal{ *workerThreadData.mainThreadSignal };

	if (!threadFiber)
	{
		threadData.result.store(ETHREAD_OP_RESULT::ERROR, std::memory_order_release);
		return false;
	}

	_fibers[localThreadIndex] = FiberType{ threadFiber };
	threadData.result = ETHREAD_OP_RESULT::OK;
	workerThreadData.startValidationCount->fetch_add(1u, std::memory_order_release);
	return true;
}

template<class KernelType>
inline void FiberTaskScheduler<KernelType>::_JoinWorkerThreads()
{
	auto const	workerThreadsCount{ _coreThreads.size() };

	for (auto index{ 1u }; index < (workerThreadsCount + 1u); ++index)
	{
		_coreThreads[index]->join();
	}
}


template<class KernelType>
bool FiberTaskScheduler<KernelType>::_Initialize(Uint8 threadCount, Uint32 fiberCount)
{
	//Checking if there is already something launched
	if (_IsRunning() || !threadCount || !fiberCount) { return false; }

	//Pre-Allocating threads data
	_threadsData.resize(threadCount);
	_coreThreads.resize(threadCount);
	_fibers.resize(threadCount + fiberCount);
	_MTExecData = UserMainThreadExecData{ threadCount, fiberCount, };
	return true;
}


}