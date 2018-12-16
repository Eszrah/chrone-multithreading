#include "scheduler/TaskPoolFunction.h"

#include "scheduler/TaskPool.h"
#include "scheduler/Task.h"
#include "scheduler/TaskBuffer.h"

#include "std_extension/SpinlockStdExt.h"
#include "AssertMacro.h"

namespace chrone::multithreading::scheduler
{

	bool 
	TaskPoolFunction::Initialize(
		TaskPool& pool,
		Uint32 maxTasksCountPowerOfTwo)
	{
		pool.headIndex = 0u;
		pool.tailIndex = 0u;
		pool.tasksMaxCount = maxTasksCountPowerOfTwo;
		pool.tasks = new Task[maxTasksCountPowerOfTwo];
		return true;
	}


	bool 
	TaskPoolFunction::Clear(
		TaskPool& pool)
	{
		pool.headIndex = 0u;
		pool.tailIndex = 0u;
		pool.tasksMaxCount = 0u;
		delete[] pool.tasks;
		pool.tasks = nullptr;
		return true;
	}


	bool 
	TaskPoolFunction::PushTasks(
		TaskPool& pool, 
		Uint32 count, 
		TaskDecl* tasksDecl, 
		const TaskDependency& dependency)
	{
		const Uint32	tasksMaxCountMinusOne{ pool.tasksMaxCount - 1u};
		Task* const		tasks{ pool.tasks };

		Task	tmpTask{};

		for (Uint32 index{ 0u }; index < count; ++index)
		{
			tmpTask = Task{ tasksDecl[index], dependency };
			LockGuardSpinLock	lock{ pool.taskBuffersLock };
			const Uint32	tailIndex{ pool.tailIndex };

			CHR_ASSERT(tailIndex != pool.headIndex);
			tasks[tailIndex] = std::move(tmpTask);
			pool.tailIndex = (tailIndex + 1u) & tasksMaxCountMinusOne;
		}


		return true;
	}


	bool 
	TaskPoolFunction::TryPopTask(
		TaskPool& pool, 
		Task& task)
	{
		const Uint32	tasksMaxCountMinusOne{ pool.tasksMaxCount - 1u };
		Task* const		tasks{ pool.tasks };
		LockGuardSpinLock	lock{ pool.taskBuffersLock };

		const Uint32	headIndex{ pool.headIndex };
		const Uint32	tailIndex{ pool.tailIndex };

		if (headIndex == tailIndex)
		{
			return false;
		}

		task = std::move(tasks[headIndex]);
		pool.headIndex = (headIndex + 1u) & tasksMaxCountMinusOne;

		return true;
	}

}

/*
	bool
	TaskPoolFunction::PushTasks(
		TaskPool& pool,
		Uint32 count,
		TaskBuffer** batches,
		const TaskDependency& dependency)
	{
		TaskBuffer*	taskBufferHead{ batches[0] };
		TaskBuffer*	taskBufferTail{ batches[count - 1] };
		TaskBatchNode*	taskBatchHead{ &taskBufferHead->taskBatch };
		TaskBatchNode&	taskBatchTail{ taskBufferTail->taskBatch };

		for (Uint32 index{ 0u }; index < count - 1; ++index)
		{
			TaskBuffer*	currentTaskBuffer{ batches[index] };
			TaskBuffer*	nextTaskBuffer{ batches[index + 1] };
			TaskBatchNode&	currentTaskBatch{ currentTaskBuffer->taskBatch };
			TaskBatchNode&	nextTaskBatch{ nextTaskBuffer->taskBatch };
			currentTaskBatch.next = &nextTaskBatch;
			currentTaskBatch.dependency = dependency;
		}

		taskBatchTail.next = nullptr;
		taskBatchTail.dependency = dependency;

		Spinlock&	lock{ pool.taskBuffersLock };
		TaskBatchNode*	taskBatchesHead{ pool.taskBatchesHead };
		TaskBatchNode*	taskBatchesTail{ pool.taskBatchesTail };

		if (taskBatchesHead)
		{
			taskBatchesTail->next = taskBatchHead;
		}
		else
		{
			taskBatchesHead = taskBatchHead;
		}

		taskBatchesTail = taskBatchHead;
		pool.taskBatchesHead = taskBatchesHead;
		pool.taskBatchesTail = taskBatchesTail;

		return true;
	}


	bool
	TaskPoolFunction::TryPopTask(
		TaskPool& pool,
		Task& task)
	{
		Uint32	taskIndex{ 0u };
		TaskBatchNode*	taskBatchesReadHead{ nullptr };

		{
			LockGuardSpinLock	lock{ pool.taskBuffersLock };
			TaskBatchNode*	taskBatchesHead{ pool.taskBatchesHead };

			taskBatchesReadHead = taskBatchesHead;

			if (!taskBatchesHead)
			{
				return false;
			}

			taskIndex = taskBatchesHead->taskCount - 1;
			taskBatchesHead->taskCount = taskIndex;

			if (taskIndex == 0u)
			{
				pool.taskBatchesHead = taskBatchesHead->next;
			}
		}

		task = Task{ taskBatchesReadHead->tasks[taskIndex],
			taskBatchesReadHead->dependency };

		return true;
	}

	bool
	TaskPoolFunction::PushTasks(
		TaskPool& pool,
		TaskNodeList pushedTaskList)
	{
		Spinlock&	spinLock{ pool.taskBuffersLock };
		TaskNodeList&	tasks{ pool.tasks };

		pushedTaskList.tail->next = nullptr;

		if (tasks.head)
		{
			tasks.tail->next = pushedTaskList.head;
		}
		else
		{
			tasks.head = pushedTaskList.head;
		}

		tasks.tail = pushedTaskList.tail;

		return true;
	}


	bool
	TaskPoolFunction::TryPopTask(
		TaskPool& pool,
		Task& task)
	{
		LockGuardSpinLock	lock{ pool.taskBuffersLock };
		TaskNodeList&	tasks{ pool.tasks };
		TaskNode*	taskshead{ tasks.head };

		if (!taskshead)
		{
			return false;
		}

		task = std::move(taskshead->task);
		taskshead = taskshead->next;
		tasks.head = taskshead;

		return true;
	}
*/