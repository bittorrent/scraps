#pragma once

#if __has_include(<any>)

#include <any>
namespace stdts {
    using std::any;
    using std::any_cast;
    using std::bad_any_cast;
    using std::swap;
} // namespace stdts

#elif __has_include(<experimental/any>)

#include <experimental/any>
namespace stdts {
    using std::experimental::any;
    using std::experimental::any_cast;
    using std::experimental::bad_any_cast;
    using std::experimental::swap;
} // namespace stdts

#else

#include <core/any.hpp>
namespace stdts {
    using core::any;
    using core::any_cast;
    using core::bad_any_cast;
    using core::swap;
} // namespace stdts

#endif
