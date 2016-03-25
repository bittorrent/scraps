#pragma once

#if __has_include(<any>)

#include <any>
namespace scraps {
namespace stdts {
    using std::any;
    using std::any_cast;
    using std::bad_any_cast;
    using std::swap;
} // namespace std
} // namespace scraps

#elif __has_include(<experimental/any>)

#include <experimental/any>
namespace scraps {
namespace stdts {
    using std::experimental::any;
    using std::experimental::any_cast;
    using std::experimental::bad_any_cast;
    using std::experimental::swap;
} // namespace std
} // namespace scraps

#else

#include <core/any.hpp>
namespace scraps {
namespace stdts {
    using core::any;
    using core::any_cast;
    using core::bad_any_cast;
    using core::swap;
} // namespace std
} // namespace scraps

#endif
