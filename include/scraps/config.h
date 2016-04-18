#pragma once

#include "scraps/platform.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <utility>

#include "scraps/warnings.h"

#include "scraps/assert.h"
#include "scraps/thread.h"
#include "scraps/logging.h"

#include "scraps/stdts/optional.h"

#define BOOST_ASIO_HAS_STD_CHRONO 1
#define BOOST_ASIO_HAS_STD_ATOMIC 1
#define BOOST_ASIO_HAS_MOVE 1
#define BOOST_ASIO_HAS_VARIADIC_TEMPLATES 1
#define BOOST_ASIO_HAS_STD_SHARED_PTR 1
#define BOOST_ASIO_HAS_STD_ARRAY 1

namespace scraps {
using namespace std::literals;
} // namespace scraps
