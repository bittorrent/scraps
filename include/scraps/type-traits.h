#pragma once

#include "scraps/config.h"

namespace scraps {

template<typename T>
struct RemoveCVR : std::remove_cv<std::remove_reference_t<T>>{};

template <typename T>
using RemoveCVRType = typename RemoveCVR<T>::type;

template <typename T, typename U>
struct EnableIfSame : std::enable_if<std::is_same<T, U>::value> {};

template <typename T, typename U>
using EnableIfSameType = typename EnableIfSame<T, U>::type;

} // namespace scraps
