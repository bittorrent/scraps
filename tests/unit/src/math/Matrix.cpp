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
#include "../tests.h"

#include "scraps/math/Matrix.h"

using namespace scraps;
using namespace scraps::math;

TEST(Matrix, translation) {
    constexpr auto matrix = Matrix<int, 4, 4>::Translation(2, 3, 4);

    STATIC_ASSERT_EQ(matrix(0, 0), 1);
    STATIC_ASSERT_EQ(matrix(0, 1), 0);
    STATIC_ASSERT_EQ(matrix(0, 2), 0);
    STATIC_ASSERT_EQ(matrix(0, 3), 2);
    STATIC_ASSERT_EQ(matrix(1, 0), 0);
    STATIC_ASSERT_EQ(matrix(1, 1), 1);
    STATIC_ASSERT_EQ(matrix(1, 2), 0);
    STATIC_ASSERT_EQ(matrix(1, 3), 3);
    STATIC_ASSERT_EQ(matrix(2, 0), 0);
    STATIC_ASSERT_EQ(matrix(2, 1), 0);
    STATIC_ASSERT_EQ(matrix(2, 2), 1);
    STATIC_ASSERT_EQ(matrix(2, 3), 4);
    STATIC_ASSERT_EQ(matrix(3, 0), 0);
    STATIC_ASSERT_EQ(matrix(3, 1), 0);
    STATIC_ASSERT_EQ(matrix(3, 2), 0);
    STATIC_ASSERT_EQ(matrix(3, 3), 1);
}

TEST(Matrix, construction) {
    constexpr Matrix<int, 3, 3> matrix{
        2, -1, 1,
        0, -2, 1,
        1, -2, 0,
    };

    STATIC_ASSERT_EQ(matrix(0, 0), 2);
    STATIC_ASSERT_EQ(matrix(0, 1), -1);
    STATIC_ASSERT_EQ(matrix(0, 2), 1);
    STATIC_ASSERT_EQ(matrix(1, 0), 0);
    STATIC_ASSERT_EQ(matrix(1, 1), -2);
    STATIC_ASSERT_EQ(matrix(1, 2), 1);
    STATIC_ASSERT_EQ(matrix(2, 0), 1);
    STATIC_ASSERT_EQ(matrix(2, 1), -2);
    STATIC_ASSERT_EQ(matrix(2, 2), 0);
}

TEST(Matrix, comparison) {
    constexpr Matrix<int, 3, 3> a{
        2, -1, 1,
        0, -2, 1,
        1, -2, 0,
    };

    constexpr Matrix<int, 3, 3> b{
        -2,  3,  5,
        -1, -3,  9,
         5,  6, -7,
    };

    constexpr Matrix<int, 3, 3> c{
        2, -1, 1,
        0, -2, 1,
        1, -2, 0,
    };

    STATIC_ASSERT_NE(a, b);
    STATIC_ASSERT_EQ(a, c);
}

TEST(Matrix, multiplication) {
    constexpr Matrix<int, 3, 3> a{
        2, -1, 1,
        0, -2, 1,
        1, -2, 0,
    };

    constexpr Matrix<int, 3, 3> b{
        -2,  3,  5,
        -1, -3,  9,
         5,  6, -7,
    };

    constexpr Matrix<int, 3, 3> expected{
        2, 15,  -6,
        7, 12, -25,
        0,  9, -13,
    };

    STATIC_ASSERT_EQ(a * b, expected);
}

TEST(Matrix, vectorMultiplication) {
    constexpr Matrix<int, 2, 3> a{
        1, -1, 2,
        0, -3, 1,
    };

    constexpr Vector<int, 3> b{2, 1, 0};

    constexpr Vector<int, 2> expected{1, -3};

    // TODO: this can become a static assert in c++17
    ASSERT_EQ(a * b, expected);
}

TEST(Matrix, determinant) {
    constexpr Matrix<int, 1, 1> a{5};
    STATIC_ASSERT_EQ(a.determinant(), 5);

    constexpr Matrix<int, 3, 3> b{
        2, -1, 1,
        0, 10, 1,
        1, -2, 0,
    };
    STATIC_ASSERT_EQ(b.determinant(), -7);

    constexpr Matrix<int, 2, 2> c{
        2, 6,
        3, 0,
    };
    STATIC_ASSERT_EQ(c.determinant(), -18);
}

TEST(Matrix, minor) {
    constexpr Matrix<int, 3, 3> a{
        1, 0, 5,
        2, 1, 6,
        3, 4, 0,
    };
    STATIC_ASSERT_EQ(a.minor(0, 1), -18);
}

TEST(Matrix, transpose) {
    constexpr Matrix<int, 2, 3> a{
        1, 0, 5,
        2, 1, 6,
    };

    constexpr Matrix<int, 3, 2> expected{
        1, 2,
        0, 1,
        5, 6,
    };

    STATIC_ASSERT_EQ(a.transpose(), expected);
}

TEST(Matrix, cofactor) {
    constexpr Matrix<int, 3, 3> a{
        2, -1, 1,
        0, 10, 1,
        1, -2, 0,
    };
    STATIC_ASSERT_EQ(a.cofactor(0, 1), 1);
}

TEST(Matrix, cofactorMatrix) {
    constexpr Matrix<int, 3, 3> a{
        2, -1, 1,
        0, 10, 1,
        1, -2, 0,
    };

    constexpr Matrix<int, 3, 3> expected{
          2,  1, -10,
         -2, -1,   3,
        -11, -2,  20,
    };

    STATIC_ASSERT_EQ(a.cofactorMatrix(), expected);
}

TEST(Matrix, adjugate) {
    constexpr Matrix<int, 3, 3> a{
        1, 2, 3,
        0, 1, 4,
        5, 6, 0,
    };

    constexpr Matrix<int, 3, 3> expected{
        -24,  18,  5,
         20, -15, -4,
         -5,   4,  1,
    };

    STATIC_ASSERT_EQ(a.adjugate(), expected);
}

TEST(Matrix, inverse) {
    constexpr Matrix<int, 3, 3> a{
        3,  3,  1,
        2,  1,  2,
        3, -2,  3,
    };

    constexpr Matrix<double, 3, 3> expected{
        1.0 / 2.0, -11.0 / 14.0,  5.0 / 14.0,
              0.0,   3.0 /  7.0, -2.0 /  7.0,
       -1.0 / 2.0,  15.0 / 14.0, -3.0 / 14.0,
    };

    STATIC_ASSERT_EQ(a.inverse(), expected);
}

TEST(Matrix, scaling) {
    constexpr Vector<int, 3> v{1, 2, 3};

    constexpr Matrix<int, 3, 3> expected{
        1, 0, 0,
        0, 2, 0,
        0, 0, 3,
    };

    constexpr auto scaling = Matrix<int, 3, 3>::Scaling(v);
    STATIC_ASSERT_EQ(scaling, expected);
}

TEST(Matrix, stream) {
    constexpr Matrix<int, 3, 3> m{
        1, 0, 0,
        0, 2, 0,
        0, 0, 3,
    };
    EXPECT_EQ(Format("{}", m), "[[1, 0, 0], [0, 2, 0], [0, 0, 3]]");
}
