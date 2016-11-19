/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "gtest.h"

#include <scraps/Byte.h>

#include <gsl.h>

#include <array>

using namespace scraps;

///////// Helper macros /////////

#define HAS_BINARY_OP(name, op)                          \
template <typename T, typename U>                        \
constexpr auto Has ## name ## Impl(T&& t, U&& u, int)    \
    -> decltype(t op u, true)                            \
{                                                        \
    return true;                                         \
}                                                        \
template <typename T, typename U>                        \
constexpr bool Has ## name ## Impl(T&& t, U&& u, long) { \
    return false;                                        \
}                                                        \
template <typename T, typename U>                        \
constexpr bool Has ## name(T&& t, U&& u) {               \
    return Has ## name ## Impl(t, u, 0);                 \
}

#define HAS_PREFIX_UNARY_OP(name, op)             \
template <typename T>                             \
constexpr auto Has ## name ## Impl(T&& t, int)    \
    -> decltype(op t, true)                       \
{                                                 \
    return true;                                  \
}                                                 \
template <typename T>                             \
constexpr bool Has ## name ## Impl(T&& t, long) { \
    return false;                                 \
}                                                 \
template <typename T>                             \
constexpr bool Has ## name(T&& t) {               \
    return Has ## name ## Impl(t, 0);             \
}

#define HAS_POSTFIX_UNARY_OP(name, op)            \
template <typename T>                             \
constexpr auto Has ## name ## Impl(T&& t, int)    \
    -> decltype(op t, true)                       \
{                                                 \
    return true;                                  \
}                                                 \
template <typename T>                             \
constexpr bool Has ## name ## Impl(T&& t, long) { \
    return false;                                 \
}                                                 \
template <typename T>                             \
constexpr bool Has ## name(T&& t) {               \
    return Has ## name ## Impl(t, 0);             \
}

#define TEST_ASSIGNMENT(type, name, lhs, op, rhs, expected) \
    constexpr type type ## _test_ ## name() {               \
        type b{lhs};                                        \
        b op##= rhs;                                        \
        return b;                                           \
    }                                                       \
    static_assert(type ## _test_ ## name() == expected, "test failed");

HAS_BINARY_OP(And,        &);
HAS_BINARY_OP(AndAssign, &=);
HAS_BINARY_OP(Or,         |);
HAS_BINARY_OP(OrAssign,  |=);
HAS_BINARY_OP(Xor,        ^);
HAS_BINARY_OP(XorAssign, ^=);
HAS_BINARY_OP(Assign,     =);
HAS_BINARY_OP(LShift,    <<);
HAS_BINARY_OP(RShift,    >>);
HAS_BINARY_OP(LSAssign, <<=);
HAS_BINARY_OP(RSAssign, >>=);
HAS_BINARY_OP(Lt,         <);
HAS_BINARY_OP(Le,        <=);
HAS_BINARY_OP(Gt,         >);
HAS_BINARY_OP(Ge,        >=);
HAS_BINARY_OP(Eq,        ==);
HAS_BINARY_OP(Neq,       !=);

HAS_PREFIX_UNARY_OP(PreIncrement, ++);

HAS_POSTFIX_UNARY_OP(PostIncrement, ++);

template <typename T>
constexpr T& LValue(T&& r) {
    return r;
}

enum class FooTag {};
using FooByte = StrongByte<FooTag>;

enum class BarTag {};
using BarByte = StrongByte<BarTag>;


///////// Sanity /////////

static_assert(std::is_same<Byte::ByteType, uint8_t>::value, "test failed");
static_assert(sizeof(Byte)    == 1, "test failed");
static_assert(sizeof(FooByte) == 1, "test failed");


///////// Construction /////////

static_assert(Byte{}           == 0x00, "test failed");
static_assert(Byte{0x01}       == 0x01, "test failed");
static_assert(Byte{Byte{0x03}} == 0x03, "test failed");

static_assert(Byte{0x00 | 0x02} == 0x02, "test failed");

static_assert(FooByte{}              == 0x00, "test failed");
static_assert(FooByte{0x01}          == 0x01, "test failed");
static_assert(FooByte{FooByte{0x03}} == 0x03, "test failed");

static_assert(FooByte{0x00 | 0x02} == 0x02, "test failed");


///////// Arithmetic /////////

static_assert(HasPreIncrement(Byte{}),     "test failed");
static_assert(HasPostIncrement(Byte{}),    "test failed");

static_assert(HasPreIncrement(FooByte{}),  "test failed");
static_assert(HasPostIncrement(FooByte{}), "test failed");


///////// Bitwise assignment operators /////////

TEST_ASSIGNMENT(Byte, xor,    0x01,  ^, 0x02, 0x03);
TEST_ASSIGNMENT(Byte, or,     0x02,  |, 0x03, 0x03);
TEST_ASSIGNMENT(Byte, and,    0x05,  &, 0x01, 0x01);
TEST_ASSIGNMENT(Byte, lshift, 0x01, <<,    1, 0x02);
TEST_ASSIGNMENT(Byte, rshift, 0x02, >>,    1, 0x01);

TEST_ASSIGNMENT(Byte, ByteXor, 0x01,  ^, Byte{0x02}, 0x03);
TEST_ASSIGNMENT(Byte, ByteOr,  0x02,  |, Byte{0x03}, 0x03);
TEST_ASSIGNMENT(Byte, ByteAnd, 0x05,  &, Byte{0x01}, 0x01);

TEST_ASSIGNMENT(Byte, FooXor,  0x01,  ^, FooByte{0x02}, 0x03);
TEST_ASSIGNMENT(Byte, FooOr,   0x02,  |, FooByte{0x03}, 0x03);
TEST_ASSIGNMENT(Byte, FooAnd,  0x05,  &, FooByte{0x01}, 0x01);

TEST_ASSIGNMENT(FooByte, xor,    0x01,  ^, 0x02, 0x03);
TEST_ASSIGNMENT(FooByte, or,     0x02,  |, 0x03, 0x03);
TEST_ASSIGNMENT(FooByte, and,    0x05,  &, 0x01, 0x01);
TEST_ASSIGNMENT(FooByte, lshift, 0x01, <<,    1, 0x02);
TEST_ASSIGNMENT(FooByte, rshift, 0x02, >>,    1, 0x01);


///////// Bitwise operators /////////

static_assert((Byte{0x01} & Byte{0x02}) == 0x00, "test failed");
static_assert((Byte{0x01} | Byte{0x02}) == 0x03, "test failed");
static_assert((Byte{0x01} ^ Byte{0x05}) == 0x04, "test failed");

static_assert(~Byte{0x00} == 0xFF, "test failed");

static_assert((FooByte{0x01} & FooByte{0x02}) == 0x00, "test failed");
static_assert((FooByte{0x01} | FooByte{0x02}) == 0x03, "test failed");
static_assert((FooByte{0x01} ^ FooByte{0x05}) == 0x04, "test failed");

static_assert(~FooByte{0x00} == 0xFF, "test failed");


///////// GenericByte and StrongByte combined operations /////////

static_assert((Byte{0x01} & FooByte{0x02}) == 0x00, "test failed");
static_assert((Byte{0x01} | FooByte{0x02}) == 0x03, "test failed");
static_assert((Byte{0x01} ^ FooByte{0x05}) == 0x04, "test failed");

static_assert((FooByte{0x01} & Byte{0x02}) == 0x00, "test failed");
static_assert((FooByte{0x01} | Byte{0x02}) == 0x03, "test failed");
static_assert((FooByte{0x01} ^ Byte{0x05}) == 0x04, "test failed");

static_assert( HasLShift  (Byte{}, 1), "test failed");
static_assert( HasLSAssign(Byte{}, 1), "test failed");
static_assert( HasRShift  (Byte{}, 1), "test failed");
static_assert( HasRSAssign(Byte{}, 1), "test failed");
static_assert( HasLShift  (FooByte{}, 1), "test failed");
static_assert( HasLSAssign(FooByte{}, 1), "test failed");
static_assert( HasRShift  (FooByte{}, 1), "test failed");
static_assert( HasRSAssign(FooByte{}, 1), "test failed");

static_assert( HasAnd       (Byte{}, FooByte{}), "test failed");
static_assert( HasAndAssign (Byte{}, FooByte{}), "test failed");
static_assert( HasOr        (Byte{}, FooByte{}), "test failed");
static_assert( HasOrAssign  (Byte{}, FooByte{}), "test failed");
static_assert( HasXor       (Byte{}, FooByte{}), "test failed");
static_assert( HasXorAssign (Byte{}, FooByte{}), "test failed");
static_assert( HasAssign    (Byte{}, FooByte{}), "test failed");
static_assert( HasLt        (Byte{}, FooByte{}), "test failed");
static_assert( HasLe        (Byte{}, FooByte{}), "test failed");
static_assert( HasGt        (Byte{}, FooByte{}), "test failed");
static_assert( HasGe        (Byte{}, FooByte{}), "test failed");
static_assert( HasEq        (Byte{}, FooByte{}), "test failed");
static_assert( HasNeq       (Byte{}, FooByte{}), "test failed");

static_assert( HasAnd       (FooByte{}, Byte{}), "test failed");
static_assert(!HasAndAssign (FooByte{}, Byte{}), "test failed");
static_assert( HasOr        (FooByte{}, Byte{}), "test failed");
static_assert(!HasOrAssign  (FooByte{}, Byte{}), "test failed");
static_assert( HasXor       (FooByte{}, Byte{}), "test failed");
static_assert(!HasXorAssign (FooByte{}, Byte{}), "test failed");
static_assert(!HasAssign    (FooByte{}, Byte{}), "test failed");
static_assert( HasLt        (FooByte{}, Byte{}), "test failed");
static_assert( HasLe        (FooByte{}, Byte{}), "test failed");
static_assert( HasGt        (FooByte{}, Byte{}), "test failed");
static_assert( HasGe        (FooByte{}, Byte{}), "test failed");
static_assert( HasEq        (FooByte{}, Byte{}), "test failed");
static_assert( HasNeq       (FooByte{}, Byte{}), "test failed");

static_assert( HasAnd       (FooByte{}, FooByte{}), "test failed");
static_assert( HasAndAssign (FooByte{}, FooByte{}), "test failed");
static_assert( HasOr        (FooByte{}, FooByte{}), "test failed");
static_assert( HasOrAssign  (FooByte{}, FooByte{}), "test failed");
static_assert( HasXor       (FooByte{}, FooByte{}), "test failed");
static_assert( HasXorAssign (FooByte{}, FooByte{}), "test failed");
static_assert( HasAssign    (FooByte{}, FooByte{}), "test failed");
static_assert( HasLt        (FooByte{}, FooByte{}), "test failed");
static_assert( HasLe        (FooByte{}, FooByte{}), "test failed");
static_assert( HasGt        (FooByte{}, FooByte{}), "test failed");
static_assert( HasGe        (FooByte{}, FooByte{}), "test failed");
static_assert( HasEq        (FooByte{}, FooByte{}), "test failed");
static_assert( HasNeq       (FooByte{}, FooByte{}), "test failed");

static_assert(!HasAnd       (BarByte{}, FooByte{}), "test failed");
static_assert(!HasAndAssign (BarByte{}, FooByte{}), "test failed");
static_assert(!HasOr        (BarByte{}, FooByte{}), "test failed");
static_assert(!HasOrAssign  (BarByte{}, FooByte{}), "test failed");
static_assert(!HasXor       (BarByte{}, FooByte{}), "test failed");
static_assert(!HasXorAssign (BarByte{}, FooByte{}), "test failed");
static_assert(!HasAssign    (BarByte{}, FooByte{}), "test failed");
static_assert(!HasLt        (BarByte{}, FooByte{}), "test failed");
static_assert(!HasLe        (BarByte{}, FooByte{}), "test failed");
static_assert(!HasGt        (BarByte{}, FooByte{}), "test failed");
static_assert(!HasGe        (BarByte{}, FooByte{}), "test failed");
static_assert(!HasEq        (BarByte{}, FooByte{}), "test failed");
static_assert(!HasNeq       (BarByte{}, FooByte{}), "test failed");

///////// Type conversions /////////

static_assert(std::is_same<decltype(Byte{0x01} & Byte{0x02}), Byte>::value, "test failed");
static_assert(std::is_same<decltype(Byte{0x01} | Byte{0x02}), Byte>::value, "test failed");
static_assert(std::is_same<decltype(Byte{0x01} ^ Byte{0x05}), Byte>::value, "test failed");

static_assert(std::is_same<decltype(   ~Byte{0x00}), Byte>::value, "test failed");
static_assert(std::is_same<decltype(~FooByte{0x00}), FooByte>::value, "test failed");

static_assert(std::is_same<decltype(   Byte{0x01} & FooByte{0x02}), Byte>::value, "test failed");
static_assert(std::is_same<decltype(   Byte{0x01} | FooByte{0x02}), Byte>::value, "test failed");
static_assert(std::is_same<decltype(   Byte{0x01} ^ FooByte{0x05}), Byte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} & FooByte{0x02}), FooByte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} | FooByte{0x02}), FooByte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} ^ FooByte{0x05}), FooByte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} &    Byte{0x02}), Byte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} |    Byte{0x02}), Byte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} ^    Byte{0x05}), Byte>::value, "test failed");

static_assert(std::is_same<decltype(   Byte{0x01} & 0xFF), Byte>::value, "test failed");
static_assert(std::is_same<decltype(   Byte{0x01} | 0xFF), Byte>::value, "test failed");
static_assert(std::is_same<decltype(   Byte{0x01} ^ 0xFF), Byte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} & 0xFF), FooByte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} | 0xFF), FooByte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} ^ 0xFF), FooByte>::value, "test failed");

static_assert(std::is_same<decltype(   Byte{0x01} << 1), Byte>::value, "test failed");
static_assert(std::is_same<decltype(   Byte{0x01} >> 1), Byte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} << 1), FooByte>::value, "test failed");
static_assert(std::is_same<decltype(FooByte{0x01} >> 1), FooByte>::value, "test failed");

static_assert(std::is_same<decltype(LValue(   Byte{0x01}) <<= 1), Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(   Byte{0x01}) >>= 1), Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(FooByte{0x01}) <<= 1), FooByte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(FooByte{0x01}) >>= 1), FooByte&>::value, "test failed");

static_assert(std::is_same<decltype(LValue(   Byte{0x01}) ^= 0xFF),          Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(   Byte{0x01}) ^= Byte{0xFF}),    Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(   Byte{0x01}) ^= FooByte{0xFF}), Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(FooByte{0x01}) ^= 0xFF),          FooByte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(FooByte{0x01}) ^= FooByte{0xFF}), FooByte&>::value, "test failed");

static_assert(std::is_same<decltype(LValue(   Byte{0x01}) |= 0xFF),          Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(   Byte{0x01}) |= Byte{0xFF}),    Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(   Byte{0x01}) |= FooByte{0xFF}), Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(FooByte{0x01}) |= 0xFF),          FooByte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(FooByte{0x01}) |= FooByte{0xFF}), FooByte&>::value, "test failed");

static_assert(std::is_same<decltype(LValue(   Byte{0x01}) &= 0xFF),          Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(   Byte{0x01}) &= Byte{0xFF}),    Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(   Byte{0x01}) &= FooByte{0xFF}), Byte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(FooByte{0x01}) &= 0xFF),          FooByte&>::value, "test failed");
static_assert(std::is_same<decltype(LValue(FooByte{0x01}) &= FooByte{0xFF}), FooByte&>::value, "test failed");

///////// Misc Usage /////////

TEST(Byte, MiscUsage) {

    {
        constexpr FooByte f{0xAB};
        constexpr Byte b{f};
        static_assert(b == 0xAB, "test failed");
    }

    {
        std::array<FooByte,     10> fooArray{{FooByte{0xAB}, FooByte{0xBC}}};
        std::array<GenericByte, 10> genericArray{};

        std::copy(fooArray.begin(), fooArray.end(), genericArray.begin());

        EXPECT_TRUE(std::equal(fooArray.begin(), fooArray.end(), genericArray.begin()));
    }

    {
        std::array<FooByte,     10> fooArray{{FooByte{0xAB}, FooByte{0xBC}}};
        std::array<GenericByte, 10> genericArray{};

        const auto fooSpan = gsl::as_span(fooArray);
        const auto genericSpan = gsl::as_span(genericArray);

        std::copy(fooSpan.begin(), fooSpan.end(), genericSpan.begin());

        EXPECT_TRUE(std::equal(fooArray.begin(), fooArray.end(), genericArray.begin()));
    }

    {
        std::array<FooByte,     10> fooArray{{FooByte{0xAB}, FooByte{0xBC}}};
        std::array<GenericByte, 10> genericArray{};

        const auto fooSpan     = gsl::as_span(fooArray);
        const auto genericSpan = gsl::as_span(genericArray);

        auto lambda = [](gsl::span<GenericByte> g){};

        lambda(genericSpan);
        lambda(genericArray);
        lambda(fooSpan);
        lambda(gsl::as_span(fooArray)); // no available conversion
    }
}

///////// Comparison /////////

//// GenericByte

// 0, 1
static_assert( (0x00  < Byte{0x01}), "test failed");
static_assert( (0x00 <= Byte{0x01}), "test failed");
static_assert(!(0x00  > Byte{0x01}), "test failed");
static_assert(!(0x00 >= Byte{0x01}), "test failed");
static_assert(!(0x00 == Byte{0x01}), "test failed");
static_assert( (0x00 != Byte{0x01}), "test failed");

// 2, 1
static_assert(!(0x02  < Byte{0x01}), "test failed");
static_assert(!(0x02 <= Byte{0x01}), "test failed");
static_assert( (0x02  > Byte{0x01}), "test failed");
static_assert( (0x02 >= Byte{0x01}), "test failed");
static_assert(!(0x02 == Byte{0x01}), "test failed");
static_assert( (0x02 != Byte{0x01}), "test failed");

// 3, 3
static_assert(!(0x03  < Byte{0x03}), "test failed");
static_assert( (0x03 <= Byte{0x03}), "test failed");
static_assert(!(0x03  > Byte{0x03}), "test failed");
static_assert( (0x03 >= Byte{0x03}), "test failed");
static_assert( (0x03 == Byte{0x03}), "test failed");
static_assert(!(0x03 != Byte{0x03}), "test failed");

// 0, 1
static_assert( (Byte{0x00}  < Byte{0x01}), "test failed");
static_assert( (Byte{0x00} <= Byte{0x01}), "test failed");
static_assert(!(Byte{0x00}  > Byte{0x01}), "test failed");
static_assert(!(Byte{0x00} >= Byte{0x01}), "test failed");
static_assert(!(Byte{0x00} == Byte{0x01}), "test failed");
static_assert( (Byte{0x00} != Byte{0x01}), "test failed");

// 2, 1
static_assert(!(Byte{0x02}  < Byte{0x01}), "test failed");
static_assert(!(Byte{0x02} <= Byte{0x01}), "test failed");
static_assert( (Byte{0x02}  > Byte{0x01}), "test failed");
static_assert( (Byte{0x02} >= Byte{0x01}), "test failed");
static_assert(!(Byte{0x02} == Byte{0x01}), "test failed");
static_assert( (Byte{0x02} != Byte{0x01}), "test failed");

// 3, 3
static_assert(!(Byte{0x03}  < Byte{0x03}), "test failed");
static_assert( (Byte{0x03} <= Byte{0x03}), "test failed");
static_assert(!(Byte{0x03}  > Byte{0x03}), "test failed");
static_assert( (Byte{0x03} >= Byte{0x03}), "test failed");
static_assert( (Byte{0x03} == Byte{0x03}), "test failed");
static_assert(!(Byte{0x03} != Byte{0x03}), "test failed");

// 0, 1
static_assert( (Byte{0x00}  < 0x01), "test failed");
static_assert( (Byte{0x00} <= 0x01), "test failed");
static_assert(!(Byte{0x00}  > 0x01), "test failed");
static_assert(!(Byte{0x00} >= 0x01), "test failed");
static_assert(!(Byte{0x00} == 0x01), "test failed");
static_assert( (Byte{0x00} != 0x01), "test failed");

// 2, 1
static_assert(!(Byte{0x02}  < 0x01), "test failed");
static_assert(!(Byte{0x02} <= 0x01), "test failed");
static_assert( (Byte{0x02}  > 0x01), "test failed");
static_assert( (Byte{0x02} >= 0x01), "test failed");
static_assert(!(Byte{0x02} == 0x01), "test failed");
static_assert( (Byte{0x02} != 0x01), "test failed");

// 3, 3
static_assert(!(Byte{0x03}  < 0x03), "test failed");
static_assert( (Byte{0x03} <= 0x03), "test failed");
static_assert(!(Byte{0x03}  > 0x03), "test failed");
static_assert( (Byte{0x03} >= 0x03), "test failed");
static_assert( (Byte{0x03} == 0x03), "test failed");
static_assert(!(Byte{0x03} != 0x03), "test failed");

//// GenericByte and StrongByte

// 0, 1
static_assert( (Byte{0x00}  < FooByte{0x01}), "test failed");
static_assert( (Byte{0x00} <= FooByte{0x01}), "test failed");
static_assert(!(Byte{0x00}  > FooByte{0x01}), "test failed");
static_assert(!(Byte{0x00} >= FooByte{0x01}), "test failed");
static_assert(!(Byte{0x00} == FooByte{0x01}), "test failed");
static_assert( (Byte{0x00} != FooByte{0x01}), "test failed");

// 2, 1
static_assert(!(Byte{0x02}  < FooByte{0x01}), "test failed");
static_assert(!(Byte{0x02} <= FooByte{0x01}), "test failed");
static_assert( (Byte{0x02}  > FooByte{0x01}), "test failed");
static_assert( (Byte{0x02} >= FooByte{0x01}), "test failed");
static_assert(!(Byte{0x02} == FooByte{0x01}), "test failed");
static_assert( (Byte{0x02} != FooByte{0x01}), "test failed");

// 3, 3
static_assert(!(Byte{0x03}  < FooByte{0x03}), "test failed");
static_assert( (Byte{0x03} <= FooByte{0x03}), "test failed");
static_assert(!(Byte{0x03}  > FooByte{0x03}), "test failed");
static_assert( (Byte{0x03} >= FooByte{0x03}), "test failed");
static_assert( (Byte{0x03} == FooByte{0x03}), "test failed");
static_assert(!(Byte{0x03} != FooByte{0x03}), "test failed");

// 0, 1
static_assert( (FooByte{0x00}  < Byte{0x01}), "test failed");
static_assert( (FooByte{0x00} <= Byte{0x01}), "test failed");
static_assert(!(FooByte{0x00}  > Byte{0x01}), "test failed");
static_assert(!(FooByte{0x00} >= Byte{0x01}), "test failed");
static_assert(!(FooByte{0x00} == Byte{0x01}), "test failed");
static_assert( (FooByte{0x00} != Byte{0x01}), "test failed");

// 2, 1
static_assert(!(FooByte{0x02}  < Byte{0x01}), "test failed");
static_assert(!(FooByte{0x02} <= Byte{0x01}), "test failed");
static_assert( (FooByte{0x02}  > Byte{0x01}), "test failed");
static_assert( (FooByte{0x02} >= Byte{0x01}), "test failed");
static_assert(!(FooByte{0x02} == Byte{0x01}), "test failed");
static_assert( (FooByte{0x02} != Byte{0x01}), "test failed");

// 3, 3
static_assert(!(FooByte{0x03}  < Byte{0x03}), "test failed");
static_assert( (FooByte{0x03} <= Byte{0x03}), "test failed");
static_assert(!(FooByte{0x03}  > Byte{0x03}), "test failed");
static_assert( (FooByte{0x03} >= Byte{0x03}), "test failed");
static_assert( (FooByte{0x03} == Byte{0x03}), "test failed");
static_assert(!(FooByte{0x03} != Byte{0x03}), "test failed");

//// StrongByte

// 0, 1
static_assert( (0x00  < FooByte{0x01}), "test failed");
static_assert( (0x00 <= FooByte{0x01}), "test failed");
static_assert(!(0x00  > FooByte{0x01}), "test failed");
static_assert(!(0x00 >= FooByte{0x01}), "test failed");
static_assert(!(0x00 == FooByte{0x01}), "test failed");
static_assert( (0x00 != FooByte{0x01}), "test failed");

// 2, 1
static_assert(!(0x02  < FooByte{0x01}), "test failed");
static_assert(!(0x02 <= FooByte{0x01}), "test failed");
static_assert( (0x02  > FooByte{0x01}), "test failed");
static_assert( (0x02 >= FooByte{0x01}), "test failed");
static_assert(!(0x02 == FooByte{0x01}), "test failed");
static_assert( (0x02 != FooByte{0x01}), "test failed");

// 3, 3
static_assert(!(0x03  < FooByte{0x03}), "test failed");
static_assert( (0x03 <= FooByte{0x03}), "test failed");
static_assert(!(0x03  > FooByte{0x03}), "test failed");
static_assert( (0x03 >= FooByte{0x03}), "test failed");
static_assert( (0x03 == FooByte{0x03}), "test failed");
static_assert(!(0x03 != FooByte{0x03}), "test failed");

// 0, 1
static_assert( (FooByte{0x00}  < FooByte{0x01}), "test failed");
static_assert( (FooByte{0x00} <= FooByte{0x01}), "test failed");
static_assert(!(FooByte{0x00}  > FooByte{0x01}), "test failed");
static_assert(!(FooByte{0x00} >= FooByte{0x01}), "test failed");
static_assert(!(FooByte{0x00} == FooByte{0x01}), "test failed");
static_assert( (FooByte{0x00} != FooByte{0x01}), "test failed");

// 2, 1
static_assert(!(FooByte{0x02}  < FooByte{0x01}), "test failed");
static_assert(!(FooByte{0x02} <= FooByte{0x01}), "test failed");
static_assert( (FooByte{0x02}  > FooByte{0x01}), "test failed");
static_assert( (FooByte{0x02} >= FooByte{0x01}), "test failed");
static_assert(!(FooByte{0x02} == FooByte{0x01}), "test failed");
static_assert( (FooByte{0x02} != FooByte{0x01}), "test failed");

// 3, 3
static_assert(!(FooByte{0x03}  < FooByte{0x03}), "test failed");
static_assert( (FooByte{0x03} <= FooByte{0x03}), "test failed");
static_assert(!(FooByte{0x03}  > FooByte{0x03}), "test failed");
static_assert( (FooByte{0x03} >= FooByte{0x03}), "test failed");
static_assert( (FooByte{0x03} == FooByte{0x03}), "test failed");
static_assert(!(FooByte{0x03} != FooByte{0x03}), "test failed");

// 0, 1
static_assert( (FooByte{0x00}  < 0x01), "test failed");
static_assert( (FooByte{0x00} <= 0x01), "test failed");
static_assert(!(FooByte{0x00}  > 0x01), "test failed");
static_assert(!(FooByte{0x00} >= 0x01), "test failed");
static_assert(!(FooByte{0x00} == 0x01), "test failed");
static_assert( (FooByte{0x00} != 0x01), "test failed");

// 2, 1
static_assert(!(FooByte{0x02}  < 0x01), "test failed");
static_assert(!(FooByte{0x02} <= 0x01), "test failed");
static_assert( (FooByte{0x02}  > 0x01), "test failed");
static_assert( (FooByte{0x02} >= 0x01), "test failed");
static_assert(!(FooByte{0x02} == 0x01), "test failed");
static_assert( (FooByte{0x02} != 0x01), "test failed");

// 3, 3
static_assert(!(FooByte{0x03}  < 0x03), "test failed");
static_assert( (FooByte{0x03} <= 0x03), "test failed");
static_assert(!(FooByte{0x03}  > 0x03), "test failed");
static_assert( (FooByte{0x03} >= 0x03), "test failed");
static_assert( (FooByte{0x03} == 0x03), "test failed");
static_assert(!(FooByte{0x03} != 0x03), "test failed");
