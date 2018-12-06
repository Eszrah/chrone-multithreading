#pragma once

#include <atomic>

#include "NativeType.h"

using TaskDeclFunc = void(*)(void*);

namespace chrone::multithreading::scheduler
{

struct TaskBuffer
{
	std::atomic<Uint>	count{ 0u };
	TaskDeclFunc		func{ nullptr };
	void**				data{ nullptr };
};

}