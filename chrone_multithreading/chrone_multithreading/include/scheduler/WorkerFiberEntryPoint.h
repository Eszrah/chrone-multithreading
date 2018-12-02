#pragma once

namespace chrone::multithreading::scheduler
{

void __stdcall WorkerFiberEntryPoint(void* data);

struct FiberEntryPointFunction
{
	static void	_Shutdown();
};

}