#pragma once

#include <cassert>

#ifdef CHR_DEBUG
#define CHR_ASSERT(expression) assert(expression)
#else
#define CHR_ASSERT(expression)
#endif // CHR_DEBUG

