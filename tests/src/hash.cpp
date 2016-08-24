#include <gtest/gtest.h>

#include "scraps/hash.h"

using namespace scraps;

static_assert("test"_fnv1a64 == 0xf9e6e6ef197c2b25ull, "fnv1a64 should produce reference hash");
static_assert(R"(
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
    this used to cause compile time evaluation to fail due to excessive recursion.\
)"_fnv1a64, "fnv1a64 should not fail for long strings");
