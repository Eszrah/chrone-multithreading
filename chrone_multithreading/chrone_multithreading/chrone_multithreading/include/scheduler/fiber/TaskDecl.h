#pragma once

#include <utility>

using TaskDeclFunc = void(*)(void*);

namespace chrone::multithreading::scheduler::fiber
{

struct TaskDecl 
{
	TaskDecl() = default;
	TaskDecl(const TaskDecl&) = delete;
	inline TaskDecl(TaskDecl&& other) = default;
	~TaskDecl() = default;

	TaskDecl&	operator=(const TaskDecl&) = delete;
	inline TaskDecl&	operator=(TaskDecl&& other) = default;

	__declspec(noinline) void operator()()
	{
		functor(data);
	}

	TaskDeclFunc	functor{ nullptr };
	void*	data{ nullptr };
};

}