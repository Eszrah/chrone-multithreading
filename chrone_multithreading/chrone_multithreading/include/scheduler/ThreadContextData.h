#pragma once

#include "NativeType.h"

namespace chrone::multithreading::scheduler
{

struct ThreadContextInfo
{
	Uint32	threadIndex{};
	//other data needed to use the scheduler in a fiber
};

}