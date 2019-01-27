#pragma once

using TaskDeclFunc = void(*)(void*);

namespace chrone::multithreading::fiberScheduler
{

struct TaskDecl 
{
	TaskDecl() = default;
	TaskDecl(const TaskDecl&) = default;
	inline TaskDecl(TaskDecl&& other) = default;
	~TaskDecl() = default;

	explicit TaskDecl(TaskDeclFunc functor, void* data):
		functor{ functor },
		data{ data }
	{}

	TaskDecl&	operator=(const TaskDecl&) = default;
	inline TaskDecl&	operator=(TaskDecl&& other) = default;

	TaskDeclFunc	functor{ nullptr };
	void*	data{ nullptr };
};

}