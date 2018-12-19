#include "scheduler/TaskBufferFunction.h"

#include "scheduler/TaskBuffer.h"
#include "scheduler/SyncPrimitive.h"


namespace chrone::multithreading::scheduler
{

void 
TaskBufferFunction::AllocateTasks(
	TaskBuffer& taskBuffer, 
	Uint32 tasksCount)
{
	taskBuffer.tasks.resize(tasksCount);
}


void 
TaskBufferFunction::RecordTasks(
	TaskBuffer& taskBuffer, 
	Uint32 offset, 
	Uint32 tasksCount, 
	TaskDeclFunc func, 
	void** data)
{
	TaskDecl*	recordedRangeBegin{ taskBuffer.tasks.data() + offset };

	for (Uint32 index{ 0u }; index < tasksCount; ++index)
	{
		recordedRangeBegin[index] = TaskDecl{ func, data[index] };
	}
}


void
TaskBufferFunction::ResetTasks(
	TaskBuffer& taskBuffer)
{
	taskBuffer.tasks.resize(0u);
}

}