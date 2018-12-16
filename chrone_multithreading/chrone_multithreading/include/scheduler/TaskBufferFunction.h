#pragma once

#include "NativeType.h"

using TaskDeclFunc = void(*)(void*);

namespace chrone::multithreading::scheduler
{

struct TaskBuffer;
struct HFence;
struct HSemaphore;

struct TaskBufferFunction
{
	static void AllocateTasks(TaskBuffer& taskBuffer, Uint32 tasksCount);
	static void RecordTasks(TaskBuffer& taskBuffer, Uint32 offset, Uint32 tasksCount, TaskDeclFunc func, void** data);
	static void ResetTasks(TaskBuffer& taskBuffer);
};

}