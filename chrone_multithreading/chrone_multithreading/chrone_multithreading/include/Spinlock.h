#pragma once

#include <atomic>
#include "NativeType.h"

namespace chrone::multithreading
{

class Spinlock
{
public:
	Spinlock() = default;
	Spinlock(const Spinlock&) = delete;
	Spinlock(Spinlock&&) = default;
	~Spinlock() = default;

	Spinlock&	operator=(const Spinlock&) = delete;
	Spinlock&	operator=(Spinlock&&) = default;

	inline void	Lock();
	inline void	Unlock();
	inline bool	TryLock();

private:

	using AtomicType = Uint8;

	std::atomic<AtomicType>	state{ 0u };
};


inline void
Spinlock::Lock()
{
	AtomicType	expectedValue{ 0u };

	while (!state.compare_exchange_weak(expectedValue, 1u, 
		std::memory_order_acquire, std::memory_order_relaxed))
	{
		expectedValue = 0u;
	}
}


inline void	
Spinlock::Unlock()
{
	state.store(0u, std::memory_order_release);
}


inline bool
Spinlock::TryLock()
{
	AtomicType	expectedValue{ 0u };
	return state.compare_exchange_strong(expectedValue, 1u, 
		std::memory_order_acquire, std::memory_order_relaxed);
}

}