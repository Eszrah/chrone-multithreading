#pragma once

namespace chrone::multithreading::fiberScheduler
{

void __stdcall WorkerFiberEntryPoint(void* data);

struct FiberEntryPointFunction
{
	static void	_Shutdown();
};

}