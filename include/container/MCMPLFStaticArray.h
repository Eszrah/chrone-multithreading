#pragma once

#include <atomic>
#include <cstring> //std::memcpy

#include "NativeType.h"

namespace chrone::multithreading::CONTAINER
{

template<
	class T,
	class Allocator>
class MCMPLFStaticArray
{
public:

	using ElementType = T;
	using AllocatorType = Allocator;

	static constexpr auto	ElementSize{ sizeof(ElementType) };

	MCMPLFStaticArray() = default;
	MCMPLFStaticArray(const MCMPLFStaticArray&) = delete;
	MCMPLFStaticArray(MCMPLFStaticArray&&) = delete;
	~MCMPLFStaticArray() = default;

	MCMPLFStaticArray&	operator=(const MCMPLFStaticArray&) = delete;
	MCMPLFStaticArray&	operator=(MCMPLFStaticArray&&) = delete;

	template<class... Args>
	bool	Resize(UInt newSize, bool isFull = false, bool ShrinkToFit = false, Args&&... args);

	bool	TryPopBack(T& element);
	bool	TryPushBack(T element);
	bool	Clear();

private:

	void	ResetIndices();

	using AtomicUInt = std::atomic<UInt>;


	AtomicUInt	_consumersIndex{ 0u };
	AtomicUInt	_producersIndex{ 0u };
	UInt	_arraySize{ 0u };

	T*	_array{ nullptr };
	AllocatorType	_allocator{};
};


template<class T, class AllocatorType>
template<class... Args>
inline bool MCMPLFStaticArray<T, AllocatorType>::Resize(UInt newSize, bool isFull, bool ShrinkToFit, Args&&... args)
{
	bool	result{ true };

	if (ShrinkToFit || newSize > size)
	{
		T*	newArray{ allocator.AllocateConstruct<T>(newSize, std::forward<Args>(args)...};

		if (!newArray) { return false; }

		std::memcpy(newArray, array, size * ElementSize);
		result = allocator.DeallocateDestruct(size, data);
		array = newArray;
	}

	size = newSize;
	topIndex = 0u;
	bottomIndex = isFull ? size : 0u;

	return result;
}

template<class T, class Allocator>
inline bool MCMPLFStaticArray<T, Allocator>::TryPopBack(T & element)
{
	UInt	currentIndex{ _consumersIndex.load(std::memory_order_relaxed) }:
	
	while (currentIndex > _consumersMaxIndex &&
		_consumersIndex.compare_exchange_weak(currentIndex, currentIndex - 1u,
			std::memory_order_release, std::memory_order_relaxed));

	if (currentIndex >= _consumersMaxIndex) { return false; }

	element = std::move(_array[currentIndex]);
	return true;
}

template<class T, class Allocator>
inline bool MCMPLFStaticArray<T, Allocator>::TryPushBack(T element)
{
	UInt	currentIndex{ _producersIndex.load(std::memory_order_relaxed) };

	while(currentIndex < _arraySize &&
		_producersIndex.compare_exchange_weak)

	return false;
}

template<class T, class AllocatorType>
inline bool MCMPLFStaticArray<T, AllocatorType>::Clear()
{
	bool	result{ true };

	if (data)
	{
		result = allocator.Deallocate(size, array);

		if (result)
		{
			array = nullptr;
			size = 0u;
			topIndex = 0u;
			bottomIndex = 0u;
		}
	}


	return result;
}

template<class T, class Allocator>
inline void MCMPLFStaticArray<T, Allocator>::ResetIndices()
{
	_consumersIndex = 0u;
	_consumersMaxIndex = 0u;
	_producersIndex = 0u;
}


}