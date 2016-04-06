#pragma once

#include "scraps/config.h"

// needed for Reverse on some specializations of rbegin and rend
#include <iterator>
#include <type_traits>

namespace scraps {
namespace detail {

template <typename R, typename = void>
class NonOwningReverseIteratorWrapper {
public:
    explicit NonOwningReverseIteratorWrapper(R& r) : _range(r) {}
    auto begin() const { return std::rbegin(_range); }
    auto begin()       { return std::rbegin(_range); }
    auto   end() const { return std::rend(_range); }
    auto   end()       { return std::rend(_range); }

private:
    R& _range;
};

template <typename R, typename = void>
class OwningReverseIteratorWrapper {
public:
    explicit OwningReverseIteratorWrapper(R&& r) : _range(r) {}
    auto begin() const { return std::rbegin(_range); }
    auto begin()       { return std::rbegin(_range); }
    auto   end() const { return std::rend(_range); }
    auto   end()       { return std::rend(_range); }

private:
    R _range;
};

} // namespace detail

/**
 * Used to provide a means to use range for loops in reverse:
 *
 * for (auto& element : Reverse(container)) {
 *    ...
 * }
 */
template <typename T>
auto Reverse(T&& t) {
    using IterType = std::conditional_t<
        std::is_rvalue_reference<T>::value,
        detail::NonOwningReverseIteratorWrapper<T>,
        detail::OwningReverseIteratorWrapper<T>
    >;
    return IterType{std::forward<T>(t)};
}

} // namespace scraps
