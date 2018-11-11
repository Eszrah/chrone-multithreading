#pragma conce

#include <atomic>
#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

template<class FunctorType>
struct Kernel
{
	Kernel() = default;
	Kernel(const Kernel&) = default;
	Kernel(Kernel&&) = default;
	~Kernel() = default;

	Kernel&	operator=(const Kernel&) = default;
	Kernel&	operator=(Kernel&&) = default;

	inline void	operator()() { functor();  }

	FunctorType	functor{};
	std::atomic<Uint16>	dependencyCount{ 1u }:
};

}