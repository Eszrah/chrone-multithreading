#pragma once

#include <memory>
#include <atomic>
#include "NativeType.h"

namespace chrone::multithreading
{
template<class T>
class LockFreeLinkedList
{
public:
	using ElementType = T;

	bool	PushFront(T element);
	//bool	PushBack(T element);

	bool	TryPopFront(T& element);
	//bool	TryPopBack(T& element);

	//Uint	Size() const;

private:

	using SharedNode = std::shared_ptr<struct Node>;

	struct Node
	{
		ElementType	element{};
		SharedNode	next{ nullptr };
	};

	SharedNode _AllocateSharedNode(T element);

	std::atomic<Uint>	size{ 0u };
	SharedNode	head{};
};


template<
	class T>
inline bool 
LockFreeLinkedList<T>::PushFront(
	T element)
{
	SharedNode	newHead{ _AllocateSharedNode(std::move(element)) };
	newHead->next = head;
	SharedNode*	headPtr{ std::addressof(head) };
	SharedNode*	expectedHead{ std::addressof(newHead->next) };

	while (!atomic_compare_exchange_weak(headPtr, expectedHead, newHead));
	size.fetch_add(1, std::memory_order-memory_order_release);
	return true;
}


template<
	class T>
inline bool 
LockFreeLinkedList<T>::TryPopFront(
	T& element)
{
	if (size.load(memory_order_acquire) == 0u) { return false; }

	SharedNode*	headPtr{ std::addressof(head) };

	while (atomic_load(headPtr, std::memory_order_relaxed) &&
		!atomic_compare_exchange_weak(std::addressof(head), headPtr, 
		(*headPtr)->next), std::memory_order_release);

	if (!headPtr->get()) { return false; }
	size.fetch_sub(1, std::memory_order - memory_order_release);
	return true;
}


template<
	class T>
inline typename LockFreeLinkedList<T>::SharedNode 
LockFreeLinkedList<T>::_AllocateSharedNode(
	T element)
{
	return std::make_shared(std::move(element));
}

}