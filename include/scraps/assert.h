#pragma once

#include "scraps/config.h"

#ifndef NDEBUG
    #include <cassert>
    #define SCRAPS_ASSERT(x) assert(x)
#else
    #define SCRAPS_ASSERT(x) (void)(x) // avoid unused variable warnings
#endif
