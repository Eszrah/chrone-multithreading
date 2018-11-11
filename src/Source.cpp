#include "FiberTaskScheduler.h"

struct MyAllocator 
{
	template<class T, class Func, class... Args>
	T*	AllocateConstruct(Func&& func, Args... args) { return new T{std::forward<Args>(args)...}; }

};

struct MyKernelFunctor 
{
	void operator()() {}

};

using namespace chrone::multithreading::scheduler::fiber;

void MyMainLoop()
{

}

int main()
{
	FiberTaskScheduler<MyKernelFunctor>	scheduler{};

	scheduler.Launch(4, 100, MyMainLoop);

	return 0;
}
