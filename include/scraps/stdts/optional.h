#pragma once

#include "scraps/config.h"

#if __has_include(<optional>)
#include <optional>
namespace scraps {
namespace stdts {
    using std::optional;
    using std::nullopt_t;
    using std::in_place_t;
    using std::bad_optional_access;
    using std::nullopt;
    using std::in_place;
    using std::make_optional;
} // namespace stdts
} // namespace scraps
#elif __has_include(<experimental/optional>)
#include <experimental/optional>
namespace scraps {
namespace stdts {
    using std::experimental::optional;
    using std::experimental::nullopt_t;
    using std::experimental::in_place_t;
    using std::experimental::bad_optional_access;
    using std::experimental::nullopt;
    using std::experimental::in_place;
    using std::experimental::make_optional;
} // namespace stdts
} // namespace scraps
#else
    #error "an implementation of optional is required!"
#endif
